
--------------------------------------------------------------

String similarity
http://www.dcs.shef.ac.uk/~sam/stringmetrics.html
http://fr.php.net/manual/en/function.levenshtein.php
MySQL : SOUNDEX()

SELECT
Id,
Surname,
SOUNDEX(Surname) AS Surname_Soundex,
DIFFERENCE(Surname, N'Johnson') AS Soundex_Difference
FROM dbo.Surnames
WHERE DIFFERENCE(Surname, N'Johnson') >= 3;

--------------------------------------------------------------

decoupage des mots
http://unicode.org/reports/tr29/tr29-11.html

--------------------------------------------------------------

stopwords : http://www.ranks.nl/stopwords/portugese.html

--------------------------------------------------------------

http://www.google.com/coop/docs/cse/resultsxml.html

--------------------------------------------------------------

passer par une table se_thesaurus et n'avoir que des id dans la
table se_words

--------------------------------------------------------------

identification d'un document : 2 cols : type + id
une clef unie les 2

le parent doit avoir le meme type 

doc_id parent_id type

--------------------------------------------------------------

donner une date a laquelle on considére que le document est vieux 

--------------------------------------------------------------

 Another factor that most people don't use when indexing is to take advantage of short indexes. You don't have to index on the entire field. Our surname and firstname fields are 40 characters each. That means the index we created above is 80 characters. Inserts to this table then also have to write an additional 80 characters, and selects have 80 character blocks to maneuvre around (disk I/O is the primary hardware bottleneck, but that's for another day!). Try reducing the size of your index - in the example above, rather use.

ALTER TABLE employee ADD INDEX(surname(20),firstname(20)); 

--------------------------------------------------------------

on insert le mot + le stem 
le stem a un poids de 0.0


------------------------------

1http://www.ranks.nl/stopwords/
