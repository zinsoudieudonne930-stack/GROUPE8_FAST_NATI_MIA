#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

enum class Niveau { L1, L2, L3 };

class Matiere {
private:
    string code;
    string libelle;
    float coefficient;

public:
    Matiere(string c, string l, float coef)
        : code(move(c)), libelle(move(l)), coefficient(coef) {}

    const string& getCode() const { return code; }
    const string& getLibelle() const { return libelle; }
    float getCoefficient() const { return coefficient; }
};

class Note {
private:
    float valeur;
    string session;
    shared_ptr<Matiere> matiere;

public:
    Note(float v, string s, shared_ptr<Matiere> m)
        : valeur(v), session(move(s)), matiere(move(m)) {}

    float getValeur() const { return valeur; }
    const Matiere& getMatiere() const { return *matiere; }
    bool estValide() const { return valeur >= 10.0f; }
};

class UE {
private:
    string code;
    string libelle;
    int credits;
    vector<shared_ptr<Matiere>> matieres;

public:
    UE(string c, string l, int cr)
        : code(move(c)), libelle(move(l)), credits(cr) {}

    void ajouterMatiere(const shared_ptr<Matiere>& matiere) {
        matieres.push_back(matiere);
    }

    float calculerMoyenne(const vector<Note>& notes) const {
        float somme = 0.0f;
        float totalCoefficients = 0.0f;

        for (const auto& note : notes) {
            auto trouvee = find_if(
                matieres.begin(),
                matieres.end(),
                [&](const shared_ptr<Matiere>& m) {
                    return m->getCode() == note.getMatiere().getCode();
                }
            );

            if (trouvee != matieres.end()) {
                somme += note.getValeur()
                       * note.getMatiere().getCoefficient();
                totalCoefficients += note.getMatiere().getCoefficient();
            }
        }

        if (totalCoefficients == 0.0f) {
            return 0.0f;
        }

        return somme / totalCoefficients;
    }

    bool estValidee(const vector<Note>& notes) const {
        return calculerMoyenne(notes) >= 10.0f;
    }

    const string& getCode() const { return code; }
    const string& getLibelle() const { return libelle; }
    int getCredits() const { return credits; }
};

class Filiere {
private:
    string code;
    string libelle;
    Niveau niveau;
    vector<shared_ptr<UE>> ues;

public:
    Filiere(string c, string l, Niveau n)
        : code(move(c)), libelle(move(l)), niveau(n) {}

    void ajouterUE(const shared_ptr<UE>& ue) {
        ues.push_back(ue);
    }

    const vector<shared_ptr<UE>>& getUEs() const {
        return ues;
    }

    const string& getLibelle() const {
        return libelle;
    }
};

class Etudiant {
private:
    int id;
    string nom;
    Niveau niveau;
    shared_ptr<Filiere> filiere;
    vector<Note> notes;

public:
    Etudiant(
        int i,
        string n,
        Niveau niv,
        shared_ptr<Filiere> f
    ) : id(i),
        nom(move(n)),
        niveau(niv),
        filiere(move(f)) {}

    void ajouterNote(const Note& note) {
        notes.push_back(note);
    }

    float calculerMoyenne() const {
        if (notes.empty()) {
            return 0.0f;
        }

        float somme = 0.0f;

        for (const auto& note : notes) {
            somme += note.getValeur();
        }

        return somme / notes.size();
    }

    bool validerUE(const UE& ue) const {
        return ue.estValidee(notes);
    }

    void afficherReleve() const {
        cout << "Releve de notes de " << nom << "\n";
        cout << "Filiere : " << filiere->getLibelle() << "\n\n";

        for (const auto& note : notes) {
            cout << note.getMatiere().getLibelle()
                 << " : "
                 << fixed << setprecision(2)
                 << note.getValeur()
                 << "\n";
        }

        cout << "Moyenne generale : "
             << fixed << setprecision(2)
             << calculerMoyenne()
             << "\n";
    }
};

int main() {
    auto programmation = make_shared<Matiere>(
        "M101",
        "Programmation C++",
        2.0f
    );

    auto algorithmique = make_shared<Matiere>(
        "M102",
        "Algorithmique",
        2.0f
    );

    auto ueProgrammation = make_shared<UE>(
        "UE101",
        "Developpement logiciel",
        5
    );

    ueProgrammation->ajouterMatiere(programmation);
    ueProgrammation->ajouterMatiere(algorithmique);

    auto licenceInfo = make_shared<Filiere>(
        "INFO",
        "Informatique",
        Niveau::L1
    );

    licenceInfo->ajouterUE(ueProgrammation);

    Etudiant etudiant(
        1,
        "Awa",
        Niveau::L1,
        licenceInfo
    );

    etudiant.ajouterNote(
        Note(14.0f, "Session normale", programmation)
    );

    etudiant.ajouterNote(
        Note(12.0f, "Session normale", algorithmique)
    );

    etudiant.afficherReleve();

    cout << "UE validee : "
         << (etudiant.validerUE(*ueProgrammation)
             ? "Oui"
             : "Non")
         << "\n";

    return 0;
}
