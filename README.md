# KWO

### Why ?
---

Test technique basé sur le framework Kernix Web Object (KWO)

### Requirements
---

- Apache 2.4
- PHP 5.6
- MySQL 5.6
- Docker
- Docker Compose

### Installation
---

Lancement des containers Docker :

```
docker-compose up -d --build
```

Création de la base de données :

```
docker-compose exec mysql bash

# Le mot de passe de la base de données se trouve dans le docker-compose.yml
mysql -u root -p test < /data/structure.sql
```

Réécriture de la configuration :

```
docker-compose exec apache bash

cp etc/app/test.conf.inc.dist etc/app/test.conf.inc

# Lancer deux fois la commande jusqu'à ne plus voir de warnings
php init.php build
php init.php build
```

Reconstruction du système de fichiers :

```
docker-compose exec apache bash

# Lancer deux fois la commande jusqu'à ne plus voir de warnings
php init.php fs.build
php init.php fs.build
```

### Documentation
---

La documentation de KWO est disponible [ici](doc/kwo4.pdf).


### Authors / Maintainers
---

- Jérôme Drzewinski
- Sofiane Souidi
