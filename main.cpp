#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>  // Pour ifstream et ofstream

using namespace std;

// Structure représentant un noeud de l'arbre de Huffman
struct Noeud {
    char donnee;
    double freq;
    Noeud *gauche, *droit;

    // Constructeur pour initialiser un noeud
    Noeud(char d, double f) {
        gauche = nullptr;
        droit = nullptr;
        donnee = d;
        freq = f;
    }
};

// Fonction pour comparer les fréquences de deux noeuds dans la file de priorité
struct compare {
    bool operator()(Noeud *gauche, Noeud *droit) {
        return gauche->freq > droit->freq;
    }
};

class Texte {
private:
    string m_chaine;
    unordered_map<char, string> codes;  // Table des codes Huffman
    Noeud* racine;  // Racine de l'arbre de Huffman

    // Fonction récursive pour générer les codes à partir de l'arbre de Huffman
    void genererCodes(Noeud* racine, string code) {
        if (!racine) return;

        /* Si c'est une feuille (un caractère)
        Ici on va parcourir tous
        */
        if (!racine->gauche && !racine->droit) {
            codes[racine->donnee] = code;
        }

        genererCodes(racine->gauche, code + "0");
        genererCodes(racine->droit, code + "1");
    }

    // Fonction qui construit l'arbre de Huffman et génère les codes
    void construireArbre(char data[], double freq[], int taille) {
        priority_queue<Noeud*, vector<Noeud*>, compare> queue;

        // Créer un noeud pour chaque caractère et l'ajouter dans la file
        for (int i = 0; i < taille; ++i) {
            queue.push(new Noeud(data[i], freq[i]));
        }

        // Construire l'arbre de Huffman
        while (queue.size() > 1) {
            Noeud *gauche = queue.top();
            queue.pop();
            Noeud *droit = queue.top();
            queue.pop();

            // Créer un nouveau noeud avec la somme des fréquences
            Noeud *somme = new Noeud('\0', gauche->freq + droit->freq);
            somme->gauche = gauche;
            somme->droit = droit;

            queue.push(somme);
        }

        // La racine de l'arbre est le seul élément restant dans la file
        racine = queue.top();

        // Générer les codes Huffman
        genererCodes(racine, "");
    }

public:
    // Constructeur qui initialise la chaîne
    Texte(string chaine) : m_chaine(chaine), racine(nullptr) {}

    // Fonction pour afficher les codes de Huffman
    void afficherCodes() {
        for (auto& pair : codes) {
            cout << pair.first << ": " << pair.second << endl;
        }
    }

    // Fonction pour encoder un texte
    string encoder() {
        string code = "";
        for (char ch : m_chaine) {
            code += codes[ch];
        }
        return code;
    }

    // Fonction pour sauvegarder les codes Huffman dans un fichier
    void sauvegarderCodes(const string& nomFichier) {
        FILE* fichier = fopen(nomFichier.c_str(), "w");
        if (fichier != nullptr) {
            for (auto& pair : codes) {
                fprintf(fichier, "%c %s\n", pair.first, pair.second.c_str());
            }
            fclose(fichier);
        }
    }

    // Fonction pour sauvegarder le texte encodé dans un fichier
    void sauvegarderTexteEncode(const string& nomFichier) {
        string texteEncode = encoder();
        FILE* fichier = fopen(nomFichier.c_str(), "w");
        if (fichier != nullptr) {
            fprintf(fichier, "%s", texteEncode.c_str());
            fclose(fichier);
        }
    }

    // Fonction pour lire les codes depuis un fichier et décoder un texte encodé
    void lireCodes(const string& fichierCodes, const string& fichierTexteEncode) {
        // Lecture des codes depuis le fichier
        FILE* fichier = fopen(fichierCodes.c_str(), "r");
        if (fichier != nullptr) {
            char ch;
            char code[256];
            codes.clear();  // On vide la table des codes avant de la recharger
            while (fscanf(fichier, " %c %s", &ch, code) != EOF) {
                codes[ch] = string(code);
            }
            fclose(fichier);
        }

        // Lecture du texte encodé depuis le fichier
        FILE* fichierEncode = fopen(fichierTexteEncode.c_str(), "r");
        if (fichierEncode != nullptr) {
            char texteEncode[1024];
            fscanf(fichierEncode, "%s", texteEncode);
            fclose(fichierEncode);

            // Décodage du texte encodé
            Noeud* current = racine;
            string texteDecode = "";
            for (int i = 0; texteEncode[i] != '\0'; ++i) {
                if (texteEncode[i] == '0') current = current->gauche;
                else current = current->droit;

                // Si on est sur une feuille, on ajoute le caractère
                if (!current->gauche && !current->droit) {
                    texteDecode += current->donnee;
                    current = racine;
                }
            }

            cout << "Texte décodé : " << texteDecode << endl;
        }
    }

    // Fonction pour initialiser l'arbre de Huffman avec un tableau de données et de fréquences
    void HuffmanCodes(char data[], double freq[], int taille) {
        construireArbre(data, freq, taille);
    }

    // Destructeur pour libérer l'arbre de Huffman
    ~Texte() {
        delete racine;
    }
};

int main() {
    char data[] = { 'a', 'b', 'c', 'd', 'e', 'f' };
    double freq[] = { 5, 9, 12, 13, 16, 45 };

    Texte texte("abcde");

    texte.HuffmanCodes(data, freq, 6);

    cout << "Codes Huffman : " << endl;
    texte.afficherCodes();

    // Sauvegarder les codes et le texte encodé dans des fichiers
    texte.sauvegarderCodes("codes.txt");
    texte.sauvegarderTexteEncode("texte_encode.txt");

    // Lire depuis les fichiers et décoder le texte
    texte.lireCodes("codes.txt", "texte_encode.txt");

    return 0;
}
