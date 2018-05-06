
### Contexte
---

Dans le cadre d'un projet TMA sous le framework KWO, vous devez intervenir sur des correctifs de bugs, des évolutions, ainsi que le développement d'une petite fonction.

L'application de test est une application simple qui liste des appartements.

Pour l'ensemble des exercices, créez d'abord votre branche si elle n'existe pas en respectant la nomenclature : `ft-<PREMIERE_LETTRE_PRENOM><NOM_ENTIER>` et faites un commit par exercice.

---
##### Exercice #1

Bug : L'application devrait maintenant être accessible à l'URL suivante : [http://localhost:8000](http://localhost) mais le développeur précédent semble avoir été négligeant ...

Rappels :

 - Point d’entrée de framework : init.php
 - Commande d’affichage des logs : php init.php log

---
##### Exercice #2

Evolution : Le client souhaiterait rajouter des filtres sur les noms des résidences et les noms des bâtiments.

---
##### Exercice #3

Bug : Le listing des logements est incomplet, l'appartement nommé "Loft" ne remonte pas.

---
##### Exercice #4

Evolution : Un nouveau modèle devra être ajouté pour associer des pièces aux logements ainsi qu'une colonne dans le tableau afin de remonter le nombre de pièces.

Une pièce a besoin des informations : nom, type (salle de bain, chambre, salon, wc), superficie.

---
##### Exercice #5

Evolution : Ajouter dans la classe "Building" une méthode qui retourne un tableau à deux niveaux avec la liste des bâtiments (niveau 1) et pour chaque bâtiment la somme des superficies par type de pièce.  

---
##### Exercice #6

Votre travail est maintenant presque terminé. Ouvrez une Pull Request afin que votre code soit revu par un membre de l'équipe de développement.
