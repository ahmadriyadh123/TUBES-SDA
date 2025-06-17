# TUBES-SDA
KELAS 1B D3 MORPHRAX
PROGRAM GAME TOWER DEFENSE
# Tower Defense Game

![Gameplay Screenshot](assets/img/gameplay_imgs/pratinjau.png)

## Deskripsi Proyek

Tower Defense Game adalah permainan strategi menara pertahanan yang dibangun menggunakan framework Raylib. Dalam game ini, pemain bertugas untuk membangun dan meng-upgrade berbagai jenis menara (tower) untuk mencegah gelombang musuh (enemies) mencapai ujung jalur. Permainan ini menawarkan fitur-fitur seperti sistem upgrade pohon yang bercabang, berbagai tipe musuh, manajemen sumber daya, dan editor level kustom.

Proyek ini dikembangkan dengan fokus pada modularitas kode C, menjadikannya mudah untuk diperluas dan dipelihara. Setiap komponen game (seperti peta, menara, musuh, UI, dan audio) diatur dalam modulnya sendiri dengan header (`.h`) dan file implementasi (`.c`) terpisah, mempromosikan desain yang bersih dan terorganisir.

## Fitur Utama

* **Sistem Gameplay Inti:** Pertahanan menara klasik di mana pemain menempatkan menara untuk menyerang gelombang musuh yang bergerak sepanjang jalur yang telah ditentukan.
* **Beragam Tipe Menara:** Tersedia berbagai tipe menara dengan kemampuan dasar yang berbeda.
* **Pohon Upgrade Menara:** Setiap menara memiliki pohon upgrade yang bercabang, memungkinkan pemain untuk mengkhususkan menara dengan efek serangan unik seperti:
    * **Serangan Kilat:** Meningkatkan kecepatan serangan secara signifikan.
    * **Serangan Berantai:** Menyerang banyak musuh secara berurutan.
    * **Serangan Area (AoE):** Merusak beberapa musuh dalam satu area.
    * **Serangan Kritikal:** Peluang untuk memberikan kerusakan lebih besar.
    * **Efek Stun:** Peluang untuk menghentikan musuh sesaat.
    * **Racun Mematikan:** Serangan meracuni musuh.
    * **Perlambat Massal:** Menerapkan efek perlambat pada area luas.
* **Sistem Ekonomi:** Kelola uang Anda dengan bijak untuk membangun dan meng-upgrade menara. Dapatkan uang dengan mengalahkan musuh.
* **Sistem Nyawa:** Lindungi pangkalan Anda! Kehilangan nyawa jika musuh berhasil mencapai ujung jalur. Permainan berakhir jika nyawa habis.
* **Menu Utama Interaktif:** Navigasi yang mudah antara game, editor level, dan pengaturan.
* **Editor Level Kustom:** Buat dan simpan peta pertahanan menara Anda sendiri dengan mudah. Tentukan jalur musuh, lokasi penempatan menara, titik awal musuh, dan jumlah gelombang.
* **Pesan Status Dinamis:** Notifikasi pop-up dalam game untuk memberikan umpan balik instan kepada pemain (misalnya, "Tower placed successfully!", "Not enough money!").
* **Manajemen Audio:** Musik latar yang dinamis (regular dan battle) serta efek suara untuk aksi dalam game (kalahkan musuh, transaksi uang).
* **Animasi Transisi Halus:** Transisi visual yang mulus antara state game yang berbeda (menu utama, gameplay, editor).

## Struktur Proyek
Tentu, saya akan bantu membuatkan README.md yang informatif, interaktif, lengkap, dan profesional untuk program Tower Defense Anda. README.md ini akan mencakup semua aspek penting dari proyek Anda.

Markdown

# Tower Defense Game

![Gameplay Screenshot](docs/images/gameplay_screenshot.png)
*(Contoh screenshot gameplay - Anda dapat menggantinya dengan screenshot aktual dari game Anda)*

## Deskripsi Proyek

Tower Defense Game adalah permainan strategi menara pertahanan yang dibangun menggunakan framework Raylib. Dalam game ini, pemain bertugas untuk membangun dan meng-upgrade berbagai jenis menara (tower) untuk mencegah gelombang musuh (enemies) mencapai ujung jalur. Permainan ini menawarkan fitur-fitur seperti sistem upgrade pohon yang bercabang, berbagai tipe musuh, manajemen sumber daya, dan editor level kustom.

Proyek ini dikembangkan dengan fokus pada modularitas kode C, menjadikannya mudah untuk diperluas dan dipelihara. Setiap komponen game (seperti peta, menara, musuh, UI, dan audio) diatur dalam modulnya sendiri dengan header (`.h`) dan file implementasi (`.c`) terpisah, mempromosikan desain yang bersih dan terorganisir.

## Fitur Utama

* **Sistem Gameplay Inti:** Pertahanan menara klasik di mana pemain menempatkan menara untuk menyerang gelombang musuh yang bergerak sepanjang jalur yang telah ditentukan.
* **Beragam Tipe Menara:** Tersedia berbagai tipe menara dengan kemampuan dasar yang berbeda.
* **Pohon Upgrade Menara:** Setiap menara memiliki pohon upgrade yang bercabang, memungkinkan pemain untuk mengkhususkan menara dengan efek serangan unik seperti:
    * **Serangan Kilat:** Meningkatkan kecepatan serangan secara signifikan.
    * **Serangan Berantai:** Menyerang banyak musuh secara berurutan.
    * **Serangan Area (AoE):** Merusak beberapa musuh dalam satu area.
    * **Serangan Kritikal:** Peluang untuk memberikan kerusakan lebih besar.
    * **Efek Stun:** Peluang untuk menghentikan musuh sesaat.
    * **Racun Mematikan:** Serangan meracuni musuh.
    * **Perlambat Massal:** Menerapkan efek perlambat pada area luas.
* **Sistem Ekonomi:** Kelola uang Anda dengan bijak untuk membangun dan meng-upgrade menara. Dapatkan uang dengan mengalahkan musuh.
* **Sistem Nyawa:** Lindungi pangkalan Anda! Kehilangan nyawa jika musuh berhasil mencapai ujung jalur. Permainan berakhir jika nyawa habis.
* **Menu Utama Interaktif:** Navigasi yang mudah antara game, editor level, dan pengaturan.
* **Editor Level Kustom:** Buat dan simpan peta pertahanan menara Anda sendiri dengan mudah. Tentukan jalur musuh, lokasi penempatan menara, titik awal musuh, dan jumlah gelombang.
* **Pesan Status Dinamis:** Notifikasi pop-up dalam game untuk memberikan umpan balik instan kepada pemain (misalnya, "Tower placed successfully!", "Not enough money!").
* **Manajemen Audio:** Musik latar yang dinamis (regular dan battle) serta efek suara untuk aksi dalam game (kalahkan musuh, transaksi uang).
* **Animasi Transisi Halus:** Transisi visual yang mulus antara state game yang berbeda (menu utama, gameplay, editor).

## Memulai Proyek

### Prasyarat

* **Raylib:** Game ini dibangun menggunakan [Raylib](https://www.raylib.com/), sebuah library game sederhana dan mudah digunakan. Pastikan Anda telah menginstal dan mengonfigurasi Raylib di sistem Anda.
    * Untuk Ubuntu/Debian: `sudo apt-get install libraylib-dev`
    * Untuk macOS (Homebrew): `brew install raylib`
    * Untuk Windows, ikuti panduan instalasi di [dokumentasi resmi Raylib](https://github.com/raysan5/raylib/wiki/Setup-and-Installation).
* **Kompiler C:** Dibutuhkan kompiler C yang kompatibel (misalnya, GCC, Clang, MinGW).
* **CMake (Opsional, Direkomendasikan):** Untuk kemudahan proses build di berbagai platform.

### Kompilasi dan Menjalankan

1.  **Kloning Repositori:**
    ```bash
    git clone https://github.com/ahmadriyadh123/TUBES-SDA
    cd TUBES-SDA
    ```

2.  **Kompilasi (menggunakan GCC/Clang secara langsung):**
    Jika Anda tidak menggunakan CMake, Anda dapat mengkompilasi secara manual. Pastikan Anda berada di direktori akar proyek.
    ```bash
    # Kompilasi semua file .c dari folder src/ dan main.c, lalu link dengan Raylib
    gcc -o game main.c tubes/src/*.c -lraylib -lm -Wno-implicit-function-declaration
    ```
    *Catatan:* Opsi `-Wno-implicit-function-declaration` mungkin diperlukan jika ada deklarasi fungsi yang implisit, namun disarankan untuk memperbaikinya di kode sumber untuk praktik terbaik.

3.  **Kompilasi (menggunakan CMake - Direkomendasikan):**
    Jika Anda memiliki CMake yang terinstal, ini adalah cara yang lebih portabel:
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```
    Setelah `make` selesai, Anda akan menemukan executable game di dalam folder `build`.

4.  **Menjalankan Game:**
    Setelah berhasil dikompilasi:
    ```bash
    ./game
    ```
    Atau jika menggunakan CMake:
    ```bash
    ./build/game
    ```

## Cara Bermain

1.  **Memulai Game:**
    * Dari **Main Menu**, pilih **Start Game**.
    * Pilih **Default Map** untuk memainkan peta bawaan, atau **Custom Map** untuk memilih peta yang Anda buat di Level Editor.
2.  **Penempatan Menara:**
    * Klik pada petak kosong yang ditandai (biasanya dengan lingkaran kosong) untuk membangun menara baru. Setiap menara membutuhkan biaya sebesar $50.
3.  **Mengalahkan Musuh:**
    * Menara Anda akan secara otomatis menyerang musuh yang berada dalam jangkauannya.
    * Setiap musuh yang dikalahkan akan memberikan sejumlah uang.
4.  **Upgrade dan Jual Menara:**
    * Klik pada menara yang sudah ditempatkan untuk membuka menu orbit.
    * Di menu ini, Anda dapat memilih untuk:
        * **Sell:** Menjual menara untuk mendapatkan kembali sebagian uang ($25).
        * **Upgrade:** Memasuki pohon upgrade menara.
            * Pilih jalur upgrade yang ingin Anda ambil (misalnya, Kecepatan Serangan, Kekuatan Serangan, Efek Khusus).
            * Klik ikon upgrade yang diinginkan. Jika memiliki biaya, ikon konfirmasi akan muncul. Klik lagi ikon konfirmasi untuk membeli upgrade.
            * Beberapa upgrade bersifat eksklusif, artinya memilih satu akan mengunci jalur lainnya pada tier yang sama.
            * Anda dapat menavigasi "Back" di pohon upgrade.
5.  **Gelombang Musuh:**
    * Musuh akan datang dalam gelombang. Perhatikan timer gelombang untuk mempersiapkan diri menghadapi serangan berikutnya. Anda bisa mempercepat gelombang dengan mengklik timer!
6.  **Kondisi Kalah:**
    * Permainan berakhir jika jumlah nyawa Anda mencapai nol (yaitu, terlalu banyak musuh yang lolos).
7.  **Kondisi Menang:**
    * Untuk custom map, permainan selesai ketika semua gelombang musuh telah dikalahkan.

## Editor Level

Editor Level memungkinkan Anda mendesain peta kustom Anda sendiri:

1.  **Akses Editor:** Dari **Main Menu**, pilih **Level Editor**.
2.  **Alat Editor:**
    * **Path Tool:** Klik untuk menempatkan atau menghapus petak jalur (`1`). Anda bisa drag-and-drop untuk menggambar jalur secara cepat.
    * **Tower Tool:** Klik untuk menempatkan atau menghapus petak penempatan menara (`4`).
    * **Start Point Tool:** Klik pada petak tepi peta (baris atau kolom 0) untuk menentukan titik awal musuh. Petak ini harus berupa bagian dari jalur.
    * **Wave Tool:** Buka panel untuk mengatur jumlah total gelombang musuh untuk peta Anda.
3.  **Menyimpan dan Memainkan Peta Kustom:**
    * Klik tombol **Save** di pojok kanan bawah editor.
    * Masukkan nama file untuk peta Anda (misalnya, `my_awesome_map`). File akan disimpan di direktori `maps/` dengan ekstensi `.txt`.
    * Setelah disimpan, peta Anda akan muncul di daftar **Custom Map** di Main Menu.

## Guide

Menu **Guide** (sebenarnya berfungsi sebagai panduan/guide dalam implementasi saat ini) menyediakan informasi penting tentang game:

* **Cara Bermain:** Petunjuk dasar tentang gameplay.
* **Daftar Skill Upgrade:** Lihat semua upgrade yang tersedia untuk menara, lengkap dengan ikon, nama, dan deskripsi efeknya.
    * Pilih skill dari daftar untuk melihat detail deskripsinya.

### Modul-modul Kunci

* `common.h`: Definisi tipe data umum (GameState, UpgradeType, UpgradeNode) yang digunakan di seluruh program.
* `map.h`/`map.c`: Mengelola grid peta, aset tile, dan data jalur.
* `enemy.h`/`enemy.c`: Definisi dan logika untuk musuh dan gelombang musuh (pergerakan, HP, animasi).
* `tower.h`/`tower.c`: Definisi dan logika untuk menara (penempatan, serangan, manajemen UI orbit).
* `upgrade_tree.h`/`upgrade_tree.c`: Mengelola struktur pohon upgrade menara dan logika penerapannya.
* `player_resources.h`/`player_resources.c`: Mengelola uang dan nyawa pemain.
* `status.h`/`status.c`: Implementasi stack untuk menampilkan pesan status dinamis.
* `main_menu.h`/`main_menu.c`: Menangani antarmuka dan logika untuk menu utama dan sub-menu.
* `level_editor.h`/`level_editor.c`: Fungsionalitas untuk membuat dan mengedit peta kustom.
* `audio.h`/`audio.c`: Mengelola semua pemuatan, pemutaran, dan pembersihan aset audio game.
* `transition.h`/`transition.c`: Mengelola animasi transisi antar state game.
* `utils.h`/`utils.c`: Fungsi-fungsi utilitas umum (misalnya, pemuatan tekstur/musik yang aman).


Tautan Proyek: [https://github.com/NamaPenggunaAnda/NamaProyekAnda](https://github.com/ahmadriyadh123/TUBES-SDA)

---
*Dibuat dengan ❤️ dan Raylib.*
