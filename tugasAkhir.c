/* kasus     : projek akhir semester
   programer : MUHAMAD AZIZAN HAKIM 25076100
   deskripsi : -
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PESANAN 100
#define MAX_MENU 50
#define MAX_NAMA 50

void identitas (void){
    printf("nama        : MUHAMAD AZIZAN HAKIM \n");
    printf("nim         : 25076100\n");
    printf("matakuliah  : PRAKTIKUM ALGORITMA PEMROGRAMAN \n");
    printf("dosen       : DRS. DENNY KURNIADI M.KOM\n");
}

// Struktur Menu (F&B & Cuci)
typedef struct {
    char nama[MAX_NAMA];
    int harga;
} Menu;

// Struktur Transaksi (Penyimpanan Pesanan)
typedef struct {
    int id_transaksi;
    char jenis[10];           // "Cuci" / "F&B"
    char nama_pelanggan[MAX_NAMA];
    int jumlah_pesanan;       // Jumlah item (untuk F&B) atau 1 (untuk cuci)
    int biaya_cuci;           // Harga dasar cuci
    int biaya_antar_jemput;   // Biaya logistik
    int biaya_ganti_oli;      // Jika ada
    int total_belanja;        // Sebelum diskon
    int diskon_didapat;       // Persentase diskon (misal: 5 = 5%)
    int total_tagihan_akhir;  // Setelah diskon
    char detail[200];         // Deskripsi pesanan
} Transaksi;

// Global variable
Transaksi riwayat[MAX_PESANAN];
int jumlah_transaksi = 0;

// Data menu makanan & minuman (tanpa ID)
Menu daftar_makanan[] = {
    {"Ayam Geprek", 10000},
    {"Minas", 10000},
    {"Nasi Goreng", 12000},
    {"Migoreng", 8000},
    {"Pecel Ayam", 15000}
};
int jumlah_makanan = 5;

Menu daftar_minuman[] = {
    {"Kopi", 8000},
    {"Air Botol Aqua 1 Liter", 8000},
    {"Jus Jeruk", 10000},
    {"Jus Mangga", 15000},
    {"Jus Buah Naga", 12000},
    {"Jus Alpukat", 12000}
};
int jumlah_minuman = 6;

// Data layanan cuci
Menu daftar_cuci[] = {
    {"Motor", 15000},
    {"Mobil", 50000}
};
int jumlah_cuci = 2;

// Tabel Diskon F&B (total belanja -> diskon %)
int diskon_fb[][2] = {
    {0, 0},      // 0 - 10.000
    {10000, 2},  // 10.000 - 20.000 → 2.5%
    {20000, 5},  // 20.000 - 30.000 → 5%
    {30000, 7},  // 30.000 - 50.000 → 7.5%
    {50000, 10}  // > 50.000 → 10%
};
int jumlah_diskon_fb = 5;

// Tabel Diskon Cuci (total belanja -> diskon %)
int diskon_cuci[][2] = {
    {0, 0},       // 0 - 100.000
    {100000, 2},  // 100.000 - 200.000 → 2.5%
    {200000, 5},  // 200.000 - 300.000 → 5%
    {300000, 7},  // 300.000 - 500.000 → 7.5%
    {500000, 10}  // > 500.000 → 10%
};
int jumlah_diskon_cuci = 5;

// Tabel Biaya Antar Jemput F&B (jarak -> biaya)
int biaya_antar_fb[][2] = {
    {0, 0},     // 0-100 meter
    {100, 2500}, // 100-200 meter
    {200, 5000}, // 200-300 meter
    {300, 7500}  // 300-400 meter
};
int jumlah_biaya_antar_fb = 4;

// Tabel Biaya Antar Jemput Cuci (untuk mobil & motor, berbeda)
int biaya_antar_motor[][2] = {
    {0, 0},      // 0-50 meter
    {50, 1500},  // 50-100 meter
    {100, 3000}, // 100-150 meter
    {150, 4500}, // 150-300 meter
    {300, 6000}, // 300-1km
    {1000, -1}   // >1km tidak tersedia
};
int jumlah_biaya_antar_motor = 6;

int biaya_antar_mobil[][2] = {
    {0, 0},      // 0-50 meter
    {50, 2500},  // 50-100 meter
    {100, 5000}, // 100-150 meter
    {150, 7500}, // 150-300 meter
    {300, 12000},// 300-1km
    {1000, -1}   // >1km tidak tersedia
};
int jumlah_biaya_antar_mobil = 6;

// Oli Motor & Mobil (tanpa ID)
Menu oli_motor[] = {
    {"AHM Oil MPX 2", 60000},
    {"Motul Scooter LE", 90000},
    {"Motul 5100 4T", 155000},
    {"Castrol Power 1 Scooter", 70000}
};
int jumlah_oli_motor = 4;

Menu oli_mobil[] = {
    {"Pertamina Fastron Techno", 350000},
    {"Castrol Magnatec", 485000},
    {"Shell Helix HX6", 385000},
    {"Shell Helix HX7", 505000}
};
int jumlah_oli_mobil = 4;

// UTILITAS

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Binary Search untuk mencari diskon berdasarkan total belanja
int cariDiskon(int total, int diskon_table[][2], int jumlah) {
    int low = 0, high = jumlah - 1;
    int diskon = 0;

    while (low <= high) {
        int mid = (low + high) / 2;
        if (total >= diskon_table[mid][0]) {
            diskon = diskon_table[mid][1];
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return diskon;
}

// Fungsi untuk menampilkan menu dengan format nomor
void tampilkanMenu(Menu *daftar, int jumlah) {
    for (int i = 0; i < jumlah; i++) {
        printf("%d. %s (Rp %d)\n", i + 1, daftar[i].nama, daftar[i].harga);
    }
}

// Fungsi untuk memilih jarak antar dan menghitung biayanya
int hitungBiayaAntar(int jarak, int biaya_table[][2], int jumlah) {
    for (int i = jumlah - 1; i >= 0; i--) {
        if (jarak >= biaya_table[i][0]) {
            return biaya_table[i][1];
        }
    }
    return 0;
}

// Merge Sort untuk mengurutkan riwayat transaksi berdasarkan total tagihan (descending)
void merge(Transaksi arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    Transaksi L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0; j = 0; k = l;
    while (i < n1 && j < n2) {
        if (L[i].total_tagihan_akhir >= R[j].total_tagihan_akhir) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++; k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++; k++;
    }
}

void mergeSort(Transaksi arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

// FUNGSI PEMBUATAN PESANAN

void buatPesananFB(void) {
    char nama_pelanggan[MAX_NAMA];
    int pilihan, jumlah_item = 0;
    int total_harga = 0;
    int biaya_antar = 0;
    int diskon = 0;
    char detail[200] = "";
    int pengambilan;

    printf("\n--- PEMBUATAN PESANAN F&B ---\n");
    printf("Masukkan nama pelanggan: ");
    scanf(" %[^\n]", nama_pelanggan);

    printf("Pilih Tipe Pengambilan:\n1. Makan di Tempat\n2. Pesan Antar\nPilih (1-2): ");
    scanf("%d", &pengambilan);

    if (pengambilan == 2) {
        int jarak;
        printf("Masukkan jarak antar (meter): ");
        scanf("%d", &jarak);
        biaya_antar = hitungBiayaAntar(jarak, biaya_antar_fb, jumlah_biaya_antar_fb);
        if (biaya_antar == -1) {
            printf("❌ Jarak terlalu jauh, layanan antar tidak tersedia.\n");
            return;
        }
        strcat(detail, "Pesan Antar (");
        char jarak_str[20];
        sprintf(jarak_str, "%d meter)", jarak);
        strcat(detail, jarak_str);
    } else {
        strcpy(detail, "Makan di Tempat");
    }

    printf("\nPilih Menu Makanan (0 untuk selesai):\n");
    tampilkanMenu(daftar_makanan, jumlah_makanan);
    do {
        printf("Pilih menu (nomor): ");
        scanf("%d", &pilihan);
        if (pilihan == 0) break;

        if (pilihan < 1 || pilihan > jumlah_makanan) {
            printf("❌ Nomor menu tidak valid.\n");
            continue;
        }

        Menu *menu = &daftar_makanan[pilihan - 1]; // -1 karena array dimulai dari 0
        total_harga += menu->harga;
        jumlah_item++;
        strcat(detail, " | ");
        strcat(detail, menu->nama);
    } while (pilihan != 0);

    printf("\nPilih Menu Minuman (0 untuk selesai):\n");
    tampilkanMenu(daftar_minuman, jumlah_minuman);
    do {
        printf("Pilih menu (nomor): ");
        scanf("%d", &pilihan);
        if (pilihan == 0) break;

        if (pilihan < 1 || pilihan > jumlah_minuman) {
            printf("❌ Nomor menu tidak valid.\n");
            continue;
        }

        Menu *menu = &daftar_minuman[pilihan - 1];
        total_harga += menu->harga;
        jumlah_item++;
        strcat(detail, " | ");
        strcat(detail, menu->nama);
    } while (pilihan != 0);

    total_harga += biaya_antar;

    // Hitung diskon
    diskon = cariDiskon(total_harga, diskon_fb, jumlah_diskon_fb);
    int diskon_rupiah = (total_harga * diskon) / 100;
    int total_akhir = total_harga - diskon_rupiah;

    // Simpan transaksi
    if (jumlah_transaksi < MAX_PESANAN) {
        riwayat[jumlah_transaksi].id_transaksi = jumlah_transaksi + 1;
        strcpy(riwayat[jumlah_transaksi].jenis, "F&B");
        strcpy(riwayat[jumlah_transaksi].nama_pelanggan, nama_pelanggan);
        riwayat[jumlah_transaksi].jumlah_pesanan = jumlah_item;
        riwayat[jumlah_transaksi].biaya_cuci = 0;
        riwayat[jumlah_transaksi].biaya_antar_jemput = biaya_antar;
        riwayat[jumlah_transaksi].biaya_ganti_oli = 0;
        riwayat[jumlah_transaksi].total_belanja = total_harga;
        riwayat[jumlah_transaksi].diskon_didapat = diskon;
        riwayat[jumlah_transaksi].total_tagihan_akhir = total_akhir;
        strcpy(riwayat[jumlah_transaksi].detail, detail);

        jumlah_transaksi++;

        printf("\n✅ Pesanan F&B berhasil dibuat!\n");
        printf("Nama Pelanggan: %s\n", nama_pelanggan);
        printf("Detail: %s\n", detail);
        printf("Total Belanja: Rp %d\n", total_harga);
        printf("Diskon: %d%% (Rp %d)\n", diskon, diskon_rupiah);
        printf("Total Tagihan Akhir: Rp %d\n", total_akhir);
    } else {
        printf("❌ Kapasitas riwayat penuh!\n");
    }
}

void buatPesananCuci(void) {
    char nama_pelanggan[MAX_NAMA];
    int pilihan, jarak_jemput = 0, jarak_antar = 0;
    int biaya_cuci = 0, biaya_antar_jemput = 0, biaya_oli = 0;
    int total_harga = 0, diskon = 0;
    char detail[200] = "";

    printf("\n--- PEMBUATAN PESANAN CUCI KENDARAAN ---\n");
    printf("Masukkan nama pelanggan: ");
    scanf(" %[^\n]", nama_pelanggan);

    printf("Pilih Jenis Kendaraan:\n1. Motor\n2. Mobil\nPilih (1-2): ");
    scanf("%d", &pilihan);

    if (pilihan < 1 || pilihan > jumlah_cuci) {
        printf("❌ Jenis kendaraan tidak valid.\n");
        return;
    }

    Menu *kendaraan = &daftar_cuci[pilihan - 1];
    biaya_cuci = kendaraan->harga;
    strcat(detail, kendaraan->nama);

    printf("Pilih Opsi Pelayanan:\n1. Dijemput\n2. Antar Sendiri\n3. Antar Jemput\nPilih (1-3): ");
    scanf("%d", &pilihan);

    if (pilihan == 1 || pilihan == 3) {
        printf("Masukkan jarak jemput (meter): ");
        scanf("%d", &jarak_jemput);
        if (strcmp(kendaraan->nama, "Motor") == 0) {
            biaya_antar_jemput += hitungBiayaAntar(jarak_jemput, biaya_antar_motor, jumlah_biaya_antar_motor);
        } else {
            biaya_antar_jemput += hitungBiayaAntar(jarak_jemput, biaya_antar_mobil, jumlah_biaya_antar_mobil);
        }
        if (biaya_antar_jemput == -1) {
            printf("❌ Jarak terlalu jauh, layanan jemput tidak tersedia.\n");
            return;
        }
        strcat(detail, " - Dijemput (");
        char jarak_str[20];
        sprintf(jarak_str, "%d meter)", jarak_jemput);
        strcat(detail, jarak_str);
    }

    if (pilihan == 2) {
        strcat(detail, " - Antar Sendiri");
    }

    if (pilihan == 3) {
        printf("Masukkan jarak antar (meter): ");
        scanf("%d", &jarak_antar);
        if (strcmp(kendaraan->nama, "Motor") == 0) {
            biaya_antar_jemput += hitungBiayaAntar(jarak_antar, biaya_antar_motor, jumlah_biaya_antar_motor);
        } else {
            biaya_antar_jemput += hitungBiayaAntar(jarak_antar, biaya_antar_mobil, jumlah_biaya_antar_mobil);
        }
        if (biaya_antar_jemput == -1) {
            printf("❌ Jarak terlalu jauh, layanan antar tidak tersedia.\n");
            return;
        }
        strcat(detail, " - Di Antar (");
        char jarak_str[20];
        sprintf(jarak_str, "%d meter)", jarak_antar);
        strcat(detail, jarak_str);
    }

    printf("Tambahkan Ganti Oli? (1=Ya, 0=Tidak): ");
    scanf("%d", &pilihan);
    if (pilihan == 1) {
        printf("Pilih Oli:\n");
        if (strcmp(kendaraan->nama, "Motor") == 0) {
            tampilkanMenu(oli_motor, jumlah_oli_motor);
            scanf("%d", &pilihan);
            if (pilihan < 1 || pilihan > jumlah_oli_motor) {
                printf("❌ Nomor oli tidak valid.\n");
                return;
            }
            Menu *oli = &oli_motor[pilihan - 1];
            biaya_oli = oli->harga;
            strcat(detail, " - Oli: ");
            strcat(detail, oli->nama);
        } else {
            tampilkanMenu(oli_mobil, jumlah_oli_mobil);
            scanf("%d", &pilihan);
            if (pilihan < 1 || pilihan > jumlah_oli_mobil) {
                printf("❌ Nomor oli tidak valid.\n");
                return;
            }
            Menu *oli = &oli_mobil[pilihan - 1];
            biaya_oli = oli->harga;
            strcat(detail, " - Oli: ");
            strcat(detail, oli->nama);
        }
    }

    total_harga = biaya_cuci + biaya_antar_jemput + biaya_oli;

    // Hitung diskon
    diskon = cariDiskon(total_harga, diskon_cuci, jumlah_diskon_cuci);
    int diskon_rupiah = (total_harga * diskon) / 100;
    int total_akhir = total_harga - diskon_rupiah;

    // Simpan transaksi
    if (jumlah_transaksi < MAX_PESANAN) {
        riwayat[jumlah_transaksi].id_transaksi = jumlah_transaksi + 1;
        strcpy(riwayat[jumlah_transaksi].jenis, "Cuci");
        strcpy(riwayat[jumlah_transaksi].nama_pelanggan, nama_pelanggan);
        riwayat[jumlah_transaksi].jumlah_pesanan = 1;
        riwayat[jumlah_transaksi].biaya_cuci = biaya_cuci;
        riwayat[jumlah_transaksi].biaya_antar_jemput = biaya_antar_jemput;
        riwayat[jumlah_transaksi].biaya_ganti_oli = biaya_oli;
        riwayat[jumlah_transaksi].total_belanja = total_harga;
        riwayat[jumlah_transaksi].diskon_didapat = diskon;
        riwayat[jumlah_transaksi].total_tagihan_akhir = total_akhir;
        strcpy(riwayat[jumlah_transaksi].detail, detail);

        jumlah_transaksi++;

        printf("\n✅ Pesanan Cuci berhasil dibuat!\n");
        printf("Nama Pelanggan: %s\n", nama_pelanggan);
        printf("Detail: %s\n", detail);
        printf("Total Belanja: Rp %d\n", total_harga);
        printf("Diskon: %d%% (Rp %d)\n", diskon, diskon_rupiah);
        printf("Total Tagihan Akhir: Rp %d\n", total_akhir);
    } else {
        printf("❌ Kapasitas riwayat penuh!\n");
    }
}

void buatPesanan(void) {
    int pilihan;
    printf("\n--- BUAT PESANAN ---\n");
    printf("1. Cuci Kendaraan\n2. F&B (Makanan & Minuman)\n");
    printf("Pilih jenis layanan (1-2): ");
    scanf("%d", &pilihan);

    if (pilihan == 1) {
        buatPesananCuci();
    } else if (pilihan == 2) {
        buatPesananFB();
    } else {
        printf("❌ Pilihan tidak valid.\n");
    }
}

// FUNGSI CEK PESANAN

void tampilkanSemuaPesanan(void) {
    if (jumlah_transaksi == 0) {
        printf("\n❌ Belum ada pesanan yang tersimpan.\n");
        return;
    }

    printf("\n=== DAFTAR SEMUA PESANAN ===\n");
    for (int i = 0; i < jumlah_transaksi; i++) {
        printf("[%d] %s | %s | Total: Rp %d | Diskon: %d%% | Akhir: Rp %d\n",
               riwayat[i].id_transaksi,
               riwayat[i].jenis,
               riwayat[i].detail,
               riwayat[i].total_belanja,
               riwayat[i].diskon_didapat,
               riwayat[i].total_tagihan_akhir);
    }
}

void cariPesananByID(void) {
    int id_cari;
    printf("Masukkan ID transaksi yang ingin dicari: ");
    scanf("%d", &id_cari);

    int ditemukan = 0;
    for (int i = 0; i < jumlah_transaksi; i++) {
        if (riwayat[i].id_transaksi == id_cari) {
            printf("\n=== DETAIL TRANSAKSI ID %d ===\n", id_cari);
            printf("Jenis: %s\n", riwayat[i].jenis);
            printf("Pelanggan: %s\n", riwayat[i].nama_pelanggan);
            printf("Detail: %s\n", riwayat[i].detail);
            printf("Total Belanja: Rp %d\n", riwayat[i].total_belanja);
            printf("Diskon: %d%%\n", riwayat[i].diskon_didapat);
            printf("Total Tagihan Akhir: Rp %d\n", riwayat[i].total_tagihan_akhir);
            ditemukan = 1;
            break;
        }
    }

    if (!ditemukan) {
        printf("❌ Transaksi dengan ID %d tidak ditemukan.\n", id_cari);
    }
}

void tampilkanRiwayatSorted(void) {
    if (jumlah_transaksi == 0) {
        printf("\n❌ Belum ada pesanan yang tersimpan.\n");
        return;
    }

    // Salin array ke temp agar tidak merusak urutan asli
    Transaksi temp[MAX_PESANAN];
    for (int i = 0; i < jumlah_transaksi; i++) {
        temp[i] = riwayat[i];
    }

    // Urutkan berdasarkan total tagihan akhir (tertinggi dulu)
    mergeSort(temp, 0, jumlah_transaksi - 1);

    printf("\n=== RIWAYAT TRANSAKSI (URUT BERDASARKAN TOTAL TAGIHAN TERTINGGI) ===\n");
    for (int i = 0; i < jumlah_transaksi; i++) {
        printf("[%d] %s | %s | Total: Rp %d | Diskon: %d%% | Akhir: Rp %d\n",
               temp[i].id_transaksi,
               temp[i].jenis,
               temp[i].detail,
               temp[i].total_belanja,
               temp[i].diskon_didapat,
               temp[i].total_tagihan_akhir);
    }
}

void cekPesanan(void) {
    int pilihan;
    printf("\n--- CEK PESANAN ---\n");
    printf("1. Tampilkan Semua Pesanan\n");
    printf("2. Cari Pesanan Berdasarkan ID\n");
    printf("3. Tampilkan Riwayat (Urut Total Tertinggi)\n");
    printf("Pilih opsi (1-3): ");
    scanf("%d", &pilihan);

    if (pilihan == 1) {
        tampilkanSemuaPesanan();
    } else if (pilihan == 2) {
        cariPesananByID();
    } else if (pilihan == 3) {
        tampilkanRiwayatSorted();
    } else {
        printf("❌ Pilihan tidak valid.\n");
    }
}

void tampilkanMenuUtama(void) {
    printf("\n========================================\n");
    printf("     LAYANAN TERPADU HIMANIKA - UNP \n");
    printf("========================================\n");
    printf("1. Buat Pesanan\n");
    printf("2. Cek Pesanan\n");
    printf("3. Keluar\n");
    printf("========================================\n");
    printf("Pilih opsi (1-3): ");
}

int main(void) {
    identitas();
    int pilihan;

    printf("SELAMAT DATANG DI LAYANAN TERPADU HIMANIKA UNP \n");

    do {
        tampilkanMenuUtama();
        scanf("%d", &pilihan);
        clearInputBuffer();

        switch(pilihan) {
            case 1:
                buatPesanan();
                break;
            case 2:
                cekPesanan();
                break;
            case 3:
                printf("\nTerima kasih telah menggunakan Layanan Terpadu HIMANIKA UNP!\n");
                break;
            default:
                printf("❌ Pilihan tidak valid. Silakan coba lagi.\n");
        }

    } while (pilihan != 3);
    return 0;
}
