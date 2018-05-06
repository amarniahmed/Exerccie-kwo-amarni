DB="kwo"

### ATTENTION ###
# la base ne doit pas être un lien

mysqldump --opt -u root $DB > $DB.sql
replace -s latin1 utf8 -- $DB.sql
#iconv -f LATIN1 -t UTF8 $DB.sql -o $DB-iconv.sql
echo "DROP DATABASE $DB; CREATE DATABASE $DB DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;" | mysql -u root
mysql -u root $DB < $DB.sql

