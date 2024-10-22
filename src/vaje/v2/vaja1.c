// tee1 

// Naloga 1

int main(int argc, char* argv) {
    if (argc !=2 && argc !=3) {
        printf("Uporaba");
    }

    char fname[256];
    if (argc == 2) {
        strcpy(fname, argv[1]);
    } else if (argc == 3) {
        strcpy(fname, argv[3]);
    }
}

// Option to open file with WRONLY | APPEND
// Ce file se ne obstaja open (CREAT)
// Ze obstaja TRUNC??


// 2. Naloga - Rep (Tail) (izpise zadnjih n-vrstic fila)
// rep <file>
// rep -m 8 <file>
// seek ( 0B, SEEK_END)
// look for new line '\n'