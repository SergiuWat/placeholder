## 1. De ce avem nevoie să instalăm Git și Git LFS?

### 🔹 Ce este Git?
Git este un **sistem de versionare**. Pe scurt:
- ne permite să lucrăm mai mulți pe același proiect
- păstrează istoricul modificărilor
- putem reveni oricând la o versiune mai veche
- evităm pierderea codului

### 🔹 Ce este Git LFS (Large File Storage)?
Git LFS este o extensie pentru Git care se ocupă de **fișiere mari** (ex: imagini, modele 3D, audio, video etc.).

Este necesar pentru că:
- Git normal nu se descurcă bine cu fișiere mari
- repository-ul rămâne mai rapid și mai organizat
- evităm erori și limite de dimensiune

👉 **Concluzie:**
Toți membrii echipei trebuie să aibă instalate **Git** și **Git LFS**.

---

## 2. Clonarea repository-ului

Repository-ul pe care îl vom folosi este:
```
https://github.com/SergiuWat/placeholder.git
```

### 🔹 Pasul 1: Deschiderea terminalului
1. Mergi în folderul unde vrei să fie proiectul (ex: `Documents` sau `Desktop`)
2. Click dreapta în folder
3. Alege **"Open in Terminal"** / **"Open Terminal Here"**

### 🔹 Pasul 2: Clonarea repository-ului
În terminal, rulează comanda:
```
git clone https://github.com/SergiuWat/placeholder.git
```

După această comandă:
- se va crea un folder numit `placeholder`
- acesta conține tot proiectul

Intră în folder dar sa te asigur ca tot esti in terminat, aceasta comanda te va ajuta sa intri cu terminalul in locatia proiectului pentru urmatorul pas unde trebuie sa activezi git lfs:
```
cd placeholder
```

---

## 3. Inițializarea Git LFS

După ce repository-ul este clonat, **trebuie inițializat Git LFS**.

În terminal, rulează:
```
git lfs install
```

Această comandă:
- activează Git LFS pe calculatorul tău
- este necesară o singură dată (de obicei)

---

## 4. Setarea username-ului și email-ului (prima instalare Git)

Dacă ai instalat Git pentru prima dată, trebuie să îți setezi identitatea.

În terminal, rulează:
```
git config --global user.name "Numele Tau"
```

```
git config --global user.email "emailul.tau@example.com"
```

🔹 Exemple:
```
git config --global user.name "Ion Popescu"
```
```
git config --global user.email "ion.popescu@gmail.com"
```

👉 Aceste date apar în istoricul de commit-uri.

---

### 5. Lucrul cu repository-ul în Visual Studio Community

### 🔹 Deschiderea proiectului
1. Deschide **Visual Studio Community**
2. Click pe **File → Open Folder**
3. Selectează folderul `placeholder`

---

## 6. FOARTE IMPORTANT: Pull înainte de orice!

⚠️ **ÎNAINTE să lucrezi pe repo sau să creezi un branch nou, TREBUIE să faci PULL**

De ce?
- ca să ai ultima versiune a proiectului
- eviți conflicte
- nu suprascrii munca altora

### 🔹 Cum faci Pull din Visual Studio Community
1. Mergi la tab-ul **Source Control** (iconița cu ramuri)
2. Click pe cele **3 puncte (⋯)**
3. Alege **Pull**

✔️ Fă acest pas:
- când deschizi proiectul
- înainte să începi să lucrezi
- înainte să creezi un branch nou

---

## 7. Crearea unui branch personal (după Pull)

După ce ai făcut **Pull** și ești sigur că ai ultima versiune a proiectului, **următorul pas obligatoriu este să îți creezi propriul branch**.

❗ NU lucra direct pe `main` / `master`.

De ce folosim branch-uri:
- fiecare lucrează izolat
- evităm conflicte
- codul poate fi verificat înainte de a ajunge pe branch-ul principal

### 🔹 Cum creezi un branch în Visual Studio Community
1. Asigură-te încă o dată că ai făcut **Pull**
2. În colțul din stânga jos, click pe numele branch-ului curent (ex: `main`)
3. Alege **Create new branch**
4. Dă un nume sugestiv branch-ului, de exemplu:
   - `feature/nume-tau`
   - `bugfix/nume-tau`

Exemplu:
```
feature/sergiu
```

După creare:
- vei fi mutat automat pe branch-ul tău
- toate commit-urile tale vor fi DOAR pe acest branch

---

## 8. Commit în Visual Studio Community

După ce faci modificări:

1. Mergi la **Git Changes**
2. Vei vedea fișierele modificate
3. Scrie un mesaj de commit (clar și scurt)
   - exemplu: `Added player movement`
4. Apasă **Commit**

👉 Commit = salvezi modificările local

---

## 8. Push în Visual Studio Community

După commit:

1. În **Git Changes**
2. Click pe sagetica care arata in sus de langa cele **3 puncte (⋯)**  **Push**

👉 Push = trimiți modificările pe GitHub, ca să le vadă și restul echipei

---

## 9. Rezumat rapid (Checklist)

✅ Git și Git LFS instalate

✅ Repository clonat
```
git clone https://github.com/SergiuWat/placeholder.git
```

✅ Git LFS inițializat
```
git lfs install
```

✅ Username și email setate

✅ **Pull înainte de lucru**

✅ **Creare branch nou (obligatoriu)**

✅ Commit → Push din Visual Studio Code

---

### 🔹 Link arhivă ZIP
👉 https://drive.google.com/file/d/1DfT8LM7PQ4NoJdZF55daKOFhNFBXBIFv/view?usp=sharing


📌 Dacă ceva nu funcționează, NU continua la întâmplare — întreabă pe Discord sau poti sa imi scrii mesaj in Privat.
Multumesc pwp :*.

