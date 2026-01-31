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

## 5. Lucrul cu repository-ul în Visual Studio Code

### 🔹 Deschiderea proiectului
1. Deschide **Visual Studio Code**
2. Click pe **File → Open Folder**
3. Selectează folderul `placeholder`

---

## 6. FOARTE IMPORTANT: Pull înainte de orice!

⚠️ **ÎNAINTE să lucrezi pe repo sau să creezi un branch nou, TREBUIE să faci PULL**

De ce?
- ca să ai ultima versiune a proiectului
- eviți conflicte
- nu suprascrii munca altora

### 🔹 Cum faci Pull din Visual Studio Code
1. Mergi la tab-ul **Source Control** (iconița cu ramuri)
2. Click pe cele **3 puncte (⋯)**
3. Alege **Pull**

✔️ Fă acest pas:
- când deschizi proiectul
- înainte să începi să lucrezi
- înainte să creezi un branch nou

---

## 7. Commit în Visual Studio Code

După ce faci modificări:

1. Mergi la **Source Control**
2. Vei vedea fișierele modificate
3. Scrie un mesaj de commit (clar și scurt)
   - exemplu: `Added player movement`
4. Apasă **Commit**

👉 Commit = salvezi modificările local

---

## 8. Push în Visual Studio Code

După commit:

1. În **Source Control**
2. Click pe **Sync Changes** sau **Push**

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

✅ Commit → Push din Visual Studio Code

---

📌 Dacă ceva nu funcționează, NU continua la întâmplare — întreabă pe Discord sau poti sa imi scrii mesaj in Privat.
Multumesc pwp :*.

