# dashDev

**Languages:**  [🇬🇧 EN](#-english) / [🇫🇷 FR](#-français)
  
---

## 🇬🇧 English

### Overview

dashDev is a unified desktop interface designed to streamline daily development activities in a salaried professional environment. It centralizes **code branch management**, **Subversion (SVN) versioning**, **Robot Framework testing**, and **auxiliary development tools** into a single, keyboard-driven workspace.

The goal of dashDev is simple: **reduce context switching**, **accelerate routine actions**, and **provide immediate visibility** on the state of projects, branches, and automated processes.

---

### Key Features

#### 🔹 Application & Branch Management

* Switch instantly between multiple applications.
* Display all branches of the selected application in a **central vertical menu**.
* Navigate entirely via the keyboard for fast workflows.

#### 🔹 Subversion Panel (Right Panel)

From the selected branch, the right-side panel provides all essential **SVN operations**, including:

* Commit changes
* Delete branches
* Check branch status
* Verify working copy integrity
* Access standard versioning actions used in day-to-day development

This panel offers a clear and immediate view of the current branch state.

#### 🔹 Tooling & CI Panel (Left Panel)

The left-side panel focuses on **supporting tools and observability**:

* Jenkins jobs execution
* Visual feedback from the Jenkins API (build status, results, failures)
* Access to log terminals
* Quick links to auxiliary development resources

This allows developers to monitor automation and diagnostics without leaving the interface.

#### 🔹 Robot Framework Integration

* Trigger Robot Framework test suites directly from the central panel
* Execute tests against the currently selected branch
* Keep testing tightly coupled with versioning and branch context

#### 🔹 Contextual Action Panel (Space Bar)

Pressing the **space bar** reveals a contextual panel that allows you to:

* Switch to another application
* Launch **Adminer**
* Open a **branch merge tool**
* Start **Poedit**

This panel acts as a fast launcher for cross-cutting tools frequently used during development.

---

### Philosophy

dashDev is built around:

* Keyboard-first navigation
* Minimal UI friction
* Clear separation of concerns (versioning, CI, testing, tooling)
* Immediate access to the most common development actions

It is not just a launcher, but a **developer control center**.
 
## Installation & Requirements
### Requirements

* Linux-based operating system (desktop environment required)
* Python 3.9 or newer
* Subversion (SVN) client
* Robot Framework
* Access to a Jenkins instance (API credentials if required)
* Git (optional, depending on project context)

### Optional / Integrated Tools

* Adminer
* Poedit
* External branch merge tools
* Terminal emulator compatible with the system

Some features depend on the availability and configuration of external tools and services (SVN repositories, Jenkins jobs, Robot Framework test suites).

## Installation

* Clone the repository
* Install Python dependencies:
`pip install -r requirements.txt

### Configure your environment:

* Define application paths
* Configure SVN repositories
* Set Jenkins endpoints and credentials
* Configure Robot Framework test locations

## Launch dashDev:
`python dashDev.py`

---

## 📄 License

To be defined.

## 👤 Author

Project developed by Sébastien Dethyre.

## 🤝 Contributions

Contributions, suggestions, and feedback are welcome.  
Feel free to open an issue or submit a pull request.

---

<br>

---

## 🇫🇷 Français

### Présentation

dashDev est une interface desktop unifiée conçue pour faciliter les activités quotidiennes de développement dans un contexte salarié. Elle centralise la **gestion des branches de code**, le **versioning Subversion (SVN)**, les **tests Robot Framework**, ainsi que de nombreux **outils annexes**, le tout dans un environnement pilotable au clavier.

L’objectif de dashDev est clair : **réduire les changements de contexte**, **accélérer les actions récurrentes** et **offrir une visibilité immédiate** sur l’état des projets, des branches et des automatisations.

---

### Fonctionnalités principales

#### 🔹 Gestion des applications et des branches

* Changement rapide entre plusieurs applications
* Affichage de toutes les branches de l’application sélectionnée dans un **menu vertical central**
* Navigation entièrement au clavier pour un flux de travail fluide

#### 🔹 Panneau Subversion (Panneau droit)

À partir de la branche sélectionnée, le panneau droit donne accès à toutes les **actions SVN essentielles** :

* Commit des modifications
* Suppression de branches
* Vérification de l’état de la branche
* Contrôle de l’intégrité de la copie de travail
* Accès aux opérations de versioning usuelles

Ce panneau fournit une vision claire et immédiate de l’état de la branche courante.

#### 🔹 Panneau outils & CI (Panneau gauche)

Le panneau gauche est dédié aux **outils de support et à la supervision** :

* Lancement de jobs Jenkins
* Visualisation des résultats via l’API Jenkins (statut, succès, échecs)
* Accès aux terminaux de logs
* Raccourcis vers les outils de diagnostic et d’analyse

Il permet de suivre l’automatisation sans quitter l’interface.

#### 🔹 Intégration Robot Framework

* Lancement direct des suites de tests Robot Framework depuis le panneau central
* Exécution des tests sur la branche actuellement sélectionnée
* Tests étroitement liés au contexte de versioning

#### 🔹 Panneau d’actions contextuelles (Barre d’espace)

Un appui sur la **barre d’espace** affiche un panneau contextuel permettant de :

* Changer d’application
* Lancer **Adminer**
* Ouvrir un outil de **merge de branches**
* Démarrer **Poedit**

Ce panneau agit comme un lanceur rapide pour les outils transverses du quotidien.

---

### Philosophie

dashDev repose sur :

* Une navigation orientée clavier
* Une interface sans friction inutile
* Une séparation claire des responsabilités (versioning, CI, tests, outils)
* Un accès immédiat aux actions les plus fréquentes

Ce n’est pas seulement un lanceur, mais un véritable **centre de contrôle pour développeur**.


## Installation et prérequis
### Prérequis

* Système Linux (environnement graphique requis)
* Python 3.9 ou supérieur
* Client Subversion (SVN)
* Robot Framework
* Accès à une instance Jenkins (identifiants API si nécessaire)
* Git (optionnel, selon le contexte projet)
 
### Outils optionnels / intégrés

* Adminer
* Poedit
* Outils externes de merge de branches
* Émulateur de terminal compatible avec le système

Certaines fonctionnalités dépendent de la disponibilité et de la configuration des outils et services externes (dépôts SVN, jobs Jenkins, suites Robot Framework).

## Installation

* Cloner le dépôt
* Installer les dépendances Python :
`pip install -r requirements.txt`

### Configurer l’environnement :

* Définir les chemins des applications
* Configurer les dépôts SVN
* Renseigner les endpoints et identifiants Jenkins
* Configurer les emplacements des tests Robot Framework

## Lancer dashDev :

`python dashDev.py`

---

## 📄 Licence

À définir.

---

## 👤 Auteur

Projet développé par Sébastien Dethyre.

---

## 🤝 Contributions

Les contributions, suggestions et retours sont les bienvenus.  
N’hésitez pas à ouvrir une issue ou une pull request.

*dashDev – One interface to rule your daily development workflow.*
