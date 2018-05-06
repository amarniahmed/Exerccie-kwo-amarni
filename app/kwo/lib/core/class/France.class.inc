<?php

class France {
  
  /* 0:code-dept 1:nom-dept 2:préfecture 3:sous-préf 4:code-région  */
  public static $departements = array('01' => array('01','Ain','Bourg-en-Bresse','Belley,Gex,Nantua','82'),
                                      '02' => array('02','Aisne','Laon','Château-Thierry,Saint-Quentin,Soissons,Vervins','22'),
                                      '03' => array('03','Allier','Moulins','Montluçon,Vichy','83'),
                                      '04' => array('04','Alpes-de-Haute-Provence','Digne','Barcelonnette,Castellane,Forcalquier','93'),
                                      '05' => array('05','Hautes-Alpes','Gap','Briançon','93'),
                                      '06' => array('06','Alpes-Maritimes','Nice','Grasse','93'),
                                      '07' => array('07','Ardèche','Privas', 'Largentière,Tournon-sur-Rhône','82'),
                                      '08' => array('08','Ardennes','Charleville-Mézières','Rethel,Sedan,Vouziers','21'),
                                      '09' => array('09','Ariège','Foix','Pamiers,Saint-Girons','73'),
                                      '10' => array('10','Aube','Troyes','Bar-sur-Aube,Nogent-sur-Seine','21'),
                                      '11' => array('11','Aude','Carcassonne','Limoux,Narbonne','91'),
                                      '12' => array('12','Aveyron','Rodez','Millau,Villefranche-de-Rouergue','73'),
                                      '13' => array('13','Bouches-du-Rhône','Marseille','Aix-en-Provence,Arles,Istres','93'),
                                      '14' => array('14','Calvados','Caen','Bayeux,Lisieux,Vire','25'),
                                      '15' => array('15','Cantal','Aurillac','Mauriac,Saint-Flour','83'),
                                      '16' => array('16','Charente','Angoulême','Cognac,Confolens','54'),
                                      '17' => array('17','Charente-Maritime','La Rochelle','Jonzac,Rochefort,Saint-Jean-d\'Angely,Saintes','54'),
                                      '18' => array('18','Cher','Bourges','Saint-Amand-Montrond,Vierzon','24'),
                                      '19' => array('19','Corrèze','Tulle','Brive-la-Gaillarde,Ussel','74'),
                                      '2A' => array('2A','Corse-du-Sud','Ajaccio','Sartène','94'),
                                      '2B' => array('2B','Haute-Corse','Bastia','Calvi,Corte','94'),
                                      '21' => array('21','Côte-d\'Or','Dijon','Beaune,Montbard','26'),
                                      '22' => array('22','Côtes-d\'Armor','St-Brieuc','Dinan,Guingamp,Lannion','53'),
                                      '23' => array('23','Creuse','Guéret','Aubusson','74'),
                                      '24' => array('24','Dordogne','Périgueux','Bergerac,Nontron,Sarlat-la-Canéda','72'),
                                      '25' => array('25','Doubs','Besançon','Montbéliard,Pontarlier','43'),
                                      '26' => array('26','Drôme','Valence','Die,Nyons','82'),
                                      '27' => array('27','Eure','Evreux','Les Andelys,Bernay','23'),
                                      '28' => array('28','Eure-et-Loir','Chartres','Châteaudun,Dreux,Nogent-le-Rotrou','24'),
                                      '29' => array('29','Finistère','Quimper','Brest,Châteaulin,Morlaix','53'),
                                      '30' => array('30','Gard','Nîmes','Alès,Le Vigan','91'),
                                      '31' => array('31','Haute-Garonne','Toulouse','Muret,Saint-Gaudens','73'),
                                      '32' => array('32','Gers','Auch','Condom,Mirande','73'),
                                      '33' => array('33','Gironde','Bordeaux','Arcachon,Blaye,Langon,Lesparre-Médoc,Libourne','72'),
                                      '34' => array('34','Hérault','Montpellier','Béziers,Lodève','91'),
                                      '35' => array('35','Ille-et-Vilaine','Rennes','Fougères,Redon,Saint-Malo','53'),
                                      '36' => array('36','Indre','Châteauroux','Le Blanc,La Châtre,Issoudun','24'),
                                      '37' => array('37','Indre-et-Loire','Tours','Chinon,Loches','24'),
                                      '38' => array('38','Isère','Grenoble','La Tour-du-Pin,Vienne','82'),
                                      '39' => array('39','Jura','Lons-le-Saunier','Dole,Saint-Claude','43'),
                                      '40' => array('40','Landes','Mont-de-Marsan','Dax','72'),
                                      '41' => array('41','Loir-et-Cher','Blois','Romorantin-Lanthenay,Vendôme','24'),
                                      '42' => array('42','Loire','St-Étienne','Montbrison,Roanne','82'),
                                      '43' => array('43','Haute-Loire','Le Puy','Brioude,Yssingeaux','83'),
                                      '44' => array('44','Loire-Atlantique','Nantes','Ancenis,Châteaubriant,Saint-Nazaire','52'),
                                      '45' => array('45','Loiret','Orléans','Montargis,Pithiviers','24'),
                                      '46' => array('46','Lot','Cahors','Figeac,Gourdon','73'),
                                      '47' => array('47','Lot-et-Garonne','Agen','Marmande,Nérac,Villeneuve-sur-Lot','72'),
                                      '48' => array('48','Lozère','Mende','Florac','91'),
                                      '49' => array('49','Maine-et-Loire','Angers','Cholet,Saumur,Segré','52'),
                                      '50' => array('50','Manche','St-Lô','Avranches,Cherbourg,Coutances','25'),
                                      '51' => array('51','Marne','Châlons-sur-Marne','Epernay,Reims,Sainte-Menehould,Vitry-le-François','21'),
                                      '52' => array('52','Haute-Marne','Chaumont','Langres,Saint-Dizier','21'),
                                      '53' => array('53','Mayenne','Laval','Château-Gontier,Mayenne','52'),
                                      '54' => array('54','Meurthe-et-Moselle','Nancy','Briey,Lunéville,Toul','41'),
                                      '55' => array('55','Meuse','Bar-le-Duc','Commercy,Verdun','41'),
                                      '56' => array('56','Morbihan','Vannes','Lorient,Pontivy','53'),
                                      '57' => array('57','Moselle','Metz','Boulay-Moselle,Château-Salins,Forbach,Sarrebourg,Sarreguemines,Thionville','41'),
                                      '58' => array('58','Nièvre','Nevers','Château-Chinon,Clamecy,Cosne-Cours-sur-Loire','26'),
                                      '59' => array('59','Nord','Lille','Avesnes-sur-Helpe,Cambrai,Douai,Dunkerque,Valenciennes','31'),
                                      '60' => array('60','Oise','Beauvais','Clermont,Compiègne,Senlis','22'),
                                      '61' => array('61','Orne','Alençon','Argentan,Mortagne-au-Perche','25'),
                                      '62' => array('62','Pas-de-Calais','Arras','Béthune,Boulogne-sur-Mer,Calais,Lens,Montreuil,Saint-Omer','31'),
                                      '63' => array('63','Puy-de-Dôme','Clermont-Ferrand','Ambert,Issoire,Riom,Thiers','83'),
                                      '64' => array('64','Pyrénées-Atlantiques','Pau','Bayonne,Oloron-Sainte-Marie','72'),
                                      '65' => array('65','Hautes-Pyrénées','Tarbes','Argelès-Gazost,Bagnères-de-Bigorre','73'),
                                      '66' => array('66','Pyrénées-Orientales','Perpignan','Céret,Prades','91'),
                                      '67' => array('67','Bas-Rhin','Strasbourg','Haguenau,Molsheim,Saverne,Sélestat,Wissembourg','42'),
                                      '68' => array('68','Haut-Rhin','Colmar','Altkirch,Guebwiller,Mulhouse,Ribeauvillé,Thann','42'),
                                      '69' => array('69','Rhône','Lyon','Villefranche-sur-Saône','82'),
                                      '70' => array('70','Haute-Saône','Vesoul','Lure','43'),
                                      '71' => array('71','Saône-et-Loire','Mâcon','Autun,Chalon-sur-Saône,Charolles,Louhans','26'),
                                      '72' => array('72','Sarthe','Le Mans','La Flèche,Mamers','52'),
                                      '73' => array('73','Savoie','Chambéry','Albertville,Saint-Jean-de-Maurienne','82'),
                                      '74' => array('74','Haute-Savoie','Annecy','Bonneville,Saint-Julien-en-Genevois,Thonon-les-Bains','82'),
                                      '75' => array('75','Paris','Paris','','11'),
                                      '76' => array('76','Seine-Maritime','Rouen','Dieppe,Le Havre','23'),
                                      '77' => array('77','Seine-et-Marne','Melun','Fontainebleau,Meaux,Provins,Torcy','11'),
                                      '78' => array('78','Yvelines','Versailles','Mantes-la-Jolie,Rambouillet,Saint-Germain-en-Laye','11'),
                                      '79' => array('79','Deux-Sèvres','Niort','Bressuire,Parthenay','54'),
                                      '80' => array('80','Somme','Amiens','Abbeville,Montdidier,Péronne','22'),
                                      '81' => array('81','Tarn','Albi','Castres','73'),
                                      '82' => array('82','Tarn-et-Garonne','Montauban','Castelsarrasin','73'),
                                      '83' => array('83','Var','Toulon','Brignoles,Draguignan','93'),
                                      '84' => array('84','Vaucluse','Avignon','Apt,Carpentras','93'),
                                      '85' => array('85','Vendée','La Roche-sur-Yon','Fontenay-le-Comte,Les Sables-d\'Olonne','52'),
                                      '86' => array('86','Vienne','Poitiers','Châtellerault,Montmorillon','54'),
                                      '87' => array('87','Haute-Vienne','Limoges','Bellac,Rochechouart','74'),
                                      '88' => array('88','Vosges','Épinal','Neufchâteau,Saint-Dié-des-Vosges','41'),
                                      '89' => array('89','Yonne','Auxerre','Avallon,Sens','26'),
                                      '90' => array('90','Territoire de Belfort','Belfort','','43'),
                                      '91' => array('91','Essonne','Évry','Etampes,Palaiseau','11'),
                                      '92' => array('92','Hauts-de-Seine','Nanterre','Antony,Boulogne-Billancourt','11'),
                                      '93' => array('93','Seine-Saint-Denis','Bobigny','Le Raincy,Saint-Denis','11'),
                                      '94' => array('94','Val-de-Marne','Créteil','L\'Haÿ-les-Roses,Nogent-sur-Marne','11'),
                                      '95' => array('95','Val-d\'Oise','Pontoise','Argenteuil,Sarcelles','11'),
                                      '971' => array('971','Guadeloupe','Basse-Terre','Pointe-à-Pitre','01'),
                                      '972' => array('972','Martinique','Fort-de-France','La Trinité,Le Marin','02'),
                                      '973' => array('973','Guyane','Cayenne','Saint-Laurent-du-Maroni','03'),
                                      '974' => array('974','La Réunion','Saint-Denis','Saint-Benoît,Saint-Paul,Saint-Pierre','04'),
                                      '976' => array('976','Mayotte','Mamutzu','','05'));

  public static $regions = array('01' => 'Guadeloupe',
                                 '02' => 'Martinique',
                                 '03' => 'Guyane',
                                 '04' => 'La Réunion',
                                 '05' => 'Mayotte',
                                 '11' => 'Île-de-France',
                                 '21' => 'Champagne-Ardenne',
                                 '22' => 'Picardie',
                                 '23' => 'Haute-Normandie',
                                 '24' => 'Centre',
                                 '25' => 'Basse-Normandie',
                                 '26' => 'Bourgogne',
                                 '31' => 'Nord-Pas-de-Calais',
                                 '41' => 'Lorraine',
                                 '42' => 'Alsace',
                                 '43' => 'Franche-Comté',
                                 '52' => 'Pays de la Loire',
                                 '53' => 'Bretagne',
                                 '54' => 'Poitou-Charentes',
                                 '72' => 'Aquitaine',
                                 '73' => 'Midi-Pyrénées',
                                 '74' => 'Limousin',
                                 '82' => 'Rhône-Alpes',
                                 '83' => 'Auvergne',
                                 '91' => 'Languedoc-Roussillon',
                                 '93' => 'Provence-Alpes-Côte d\'Azur',
                                 '94' => 'Corse');
  
  // pop > 10000
  public static $villes = array('01' => array('Bourg-en-Bresse,Oyonnax,Amberieu-en-Bugey,Bellegarde-sur-Valserine','46.1329075','5.51262'),
                                '02' => array('Saint-Quentin,Soissons,Laon,Chateau-Thierry,Tergnier,Chauny,Hirson,Villers-Cotterets','49.53677125','3.41501375'),
                                '03' => array('Montlucon,Vichy,Moulins,Cusset,Saint-Francois,Yzeure','46.354408333333','3.345'),
                                '04' => array('Manosque,Digne-les-Bains','43.960675','6.009435'),
                                '05' => array('Gap,Briancon','44.733335','6.366665'),
                                '06' => array('Nice,Antibes,Cannes,Grasse,Cagnes-sur-Mer,Le Cannet,Menton,Saint-Laurent-du-Var,Vallauris,Mandelieu-la-Napoule,Mougins,Vence,Villeneuve-Loubet,Beausoleil,Valbonne,Roquebrune-Cap-Martin,Carros,La Trinite','43.664677222222','7.1553755555556'),
                                '07' => array('Annonay,Aubenas,Guilherand-Granges','44.930846666667','4.6448033333333'),
                                '08' => array('Charleville-Mezieres,Sedan','49.733335','4.833335'),
                                '09' => array('Pamiers','43.11667','1.6'),
                                '10' => array('Troyes,La Chapelle-Saint-Luc,Romilly-sur-Seine,Saint-Andre-les-Vergers,Sainte-Savine','48.341966','3.989614'),
                                '11' => array('Narbonne,Carcassonne,Castelnaudary,Limoux','43.19329','2.381555'),
                                '12' => array('Rodez,Millau,Villefranche-de-Rouergue','44.26808','2.5645766666667'),
                                '13' => array('Marseille,Aix-en-Provence,Arles,Martigues,Aubagne,Istres,Salon-de-Provence,Vitrolles,Marignane,La Ciotat,Miramas,Allauch,Gardanne,Les Pennes-Mirabeau,Fos-sur-Mer,Port-de-Bouc,Tarascon,Chateaurenard,Berre-l\'Etang,Bouc-Bel-Air,Rognac,Saint-Martin-de-Crau,Chateauneuf-les-Martigues,Plan-de-Cuques,Saint-Remy-de-Provence,Septemes-les-Vallons','43.488649615385','5.1692792307692'),
                                '14' => array('Caen,Lisieux,Herouville-Saint-Clair,Bayeux,Vire,Mondeville','49.138158333333','-0.39100166666667'),
                                '15' => array('Aurillac','44.91667','2.45'),
                                '16' => array('Angouleme,Cognac,Soyaux','45.666666666667','0.0055566666666667'),
                                '17' => array('La Rochelle,Saintes,Rochefort,Royan','45.869615','-0.94869'),
                                '18' => array('Bourges,Vierzon,Saint-Amand-Montrond','47.005556666667','2.3333333333333'),
                                '19' => array('Brive-la-Gaillarde,Tulle,Ussel','45.322223333333','1.8722233333333'),
                                '21' => array('Dijon,Beaune,Chenove,Talant,Chevigny-Saint-Sauveur,Quetigny','47.265986666667','5.018965'),
                                '22' => array('Saint-Brieuc,Lannion,Plerin,Lamballe,Dinan,Ploufragan,Loudeac','48.482825714286','-2.7323485714286'),
                                '23' => array('Gueret','46.16667','1.86667'),
                                '24' => array('Perigueux,Bergerac,Sarlat-la-Caneda','44.974116666667','0.80552'),
                                '25' => array('Besancon,Montbeliard,Pontarlier,Audincourt,Valentigney','47.323214','6.571732'),
                                '26' => array('Valence,Montelimar,Romans-sur-Isere,Bourg-les-Valence,Pierrelatte,Bourg-de-Peage','44.816653333333','4.898495'),
                                '27' => array('Evreux,Vernon,Louviers,Val-de-Reuil,Bernay,Gisors','49.16304','1.2282833333333'),
                                '28' => array('Chartres,Dreux,Luce,Chateaudun,Nogent-le-Rotrou,Vernouillet,Mainvilliers','48.455431428571','1.33025'),
                                '29' => array('Brest,Quimper,Concarneau,Morlaix,Douarnenez,Landerneau,Guipavas,Plougastel-Daoulas,Plouzane,Quimperle,Le Relecq-Kerhuon','48.260462727273','-4.2040745454545'),
                                '2A' => array('Ajaccio,Porto-Vecchio','41.75912','9.007045'),
                                '2B' => array('Bastia','42.70278','9.45'),
                                '30' => array('Nimes,Ales,Bagnols-sur-Ceze,Beaucaire,Villeneuve-les-Avignon,Saint-Gilles,Vauvert,Pont-Saint-Esprit','43.94122625','4.48077625'),
                                '31' => array('Toulouse,Colomiers,Tournefeuille,Muret,Blagnac,Plaisance-du-Touch,Balma,Cugnaux,Saint-Orens-de-Gameville,Ramonville-Saint-Agne,Saint-Gaudens,L\'Union,Castanet-Tolosan','43.539323076923','1.3618846153846'),
                                '32' => array('Auch','43.65','0.58333'),
                                '33' => array('Bordeaux,Merignac,Pessac,Talence,Villenave-d\'Ornon,Saint-Medard-en-Jalles,La Teste-de-Buch,Gradignan,Libourne,Le Bouscat,Begles,Lormont,Cenon,Eysines,Gujan-Mestras,Cestas,Blanquefort,Floirac,Ambares-et-Lagrave,Bruges,Arcachon,Andernos-les-Bains','44.812667727273','-0.67235954545455'),
                                '34' => array('Montpellier,Beziers,Sete,Lunel,Agde,Frontignan,Mauguio,Castelnau-le-Lez,Lattes','43.512926666667','3.7745277777778'),
                                '35' => array('Rennes,Saint-Malo,Fougeres,Vitre,Cesson-Sevigne,Bruz,Dinard','48.286652857143','-1.6438685714286'),
                                '36' => array('Chateauroux,Issoudun','46.883335','1.85'),
                                '37' => array('Tours,Joue-les-Tours,Saint-Cyr-sur-Loire,Saint-Avertin,Saint-Pierre-des-Corps,Amboise,Chambray-les-Tours,Montlouis-sur-Loire','47.37770875','0.753135'),
                                '38' => array('Grenoble,Saint-Martin-d\'Heres,Echirolles,Vienne,Bourgoin,Fontaine,Bourgoin-Jallieu,Voiron,Meylan,Villefontaine,Saint-Egreve,Seyssinet-Pariset,L\'Isle-d\'Abeau,Le Pont-de-Claix,Sassenage,Voreppe,Eybens','45.315180588235','5.54174'),
                                '39' => array('Dole,Lons-le-Saunier,Valfin-les-Saint-Claude,Saint-Claude','46.6489725','5.6923575'),
                                '40' => array('Mont-de-Marsan,Dax,Saint-Paul-les-Dax,Tarnos,Biscarrosse','43.852786','-1.046486'),
                                '41' => array('Blois,Romorantin-Lanthenay,Vendome','47.583333333333','1.3833333333333'),
                                '42' => array('Saint-Etienne,Roanne,Saint-Chamond,Firminy,Montbrison,Rive-de-Gier,Saint-Just-Saint-Rambert,Le Chambon-Feugerolles,Riorges,Roche-la-Moliere','45.583079','4.289388'),
                                '43' => array('Le Puy-en-Velay','45.04366','3.88523'),
                                '44' => array('Nantes,Saint-Nazaire,Saint-Herblain,Reze,Saint-Sebastien-sur-Loire,Orvault,Vertou,Coueron,La Chapelle-sur-Erdre,Carquefou,La Baule-Escoublac,Bouguenais,Guerande,Sainte-Luce-sur-Loire,Chateaubriant,Pornic,Pornichet,Saint-Brevin-les-Pins','47.264134444444','-1.7880977777778'),
                                '45' => array('Orleans,Olivet,Fleury-les-Aubrais,Saint-Jean-de-Braye,Saint-Jean-de-la-Ruelle,Montargis,Saran,Gien,Chalette-sur-Loing','47.908831111111','2.1728666666667'),
                                '46' => array('Cahors,Figeac','44.52455','1.73498'),
                                '47' => array('Agen,Villeneuve-sur-Lot,Marmande','44.369513333333','0.50138333333333'),
                                '48' => array('Mende','44.51667','3.5'),
                                '49' => array('Angers,Cholet,Saumur,Avrille,Les Ponts-de-Ce,Trelaze','47.361468333333','-0.51502666666667'),
                                '50' => array('Cherbourg-Octeville,Saint-Lo,Equeurdreville-Hainneville,Tourlaville,Octeville,Granville','49.417846666667','-1.5277916666667'),
                                '51' => array('Reims,Chalons-en-Champagne,Epernay,Vitry-le-Francois,Tinqueux','49.047452','4.183446'),
                                '52' => array('Saint-Dizier,Chaumont','48.375','5.041665'),
                                '53' => array('Laval,Mayenne,Chateau-Gontier','48.066666666667','-0.69444666666667'),
                                '54' => array('Nancy,Vandoeuvre-les-Nancy,Luneville,Toul,Laxou,Villers-les-Nancy,Pont-a-Mousson,Longwy,Saint-Max,Jarville-la-Malgrange','48.777142','6.132623'),
                                '55' => array('Verdun,Bar-le-Duc','48.975','5.275'),
                                '56' => array('Lorient,Vannes,Lanester,Ploemeur,Pontivy,Hennebont,Auray','47.779051428571','-3.1625971428571'),
                                '57' => array('Metz,Thionville,Montigny-les-Metz,Sarreguemines,Forbach,Saint-Avold,Yutz,Hayange,Sarrebourg,Freyming-Merlebach,Creutzwald,Woippy,Stiring-Wendel,Fameck,Marly,Rombas,Florange,Behren-les-Forbach,Amneville','49.182402631579','6.4506705263158'),
                                '58' => array('Nevers,Cosne-Cours-sur-Loire,Varennes-Vauzelles','47.139116666667','3.0748833333333'),
                                '59' => array('Lille,Roubaix,Tourcoing,Dunkerque,Villeneuve-d\'Ascq,Douai,Valenciennes,Wattrelos,Marcq-en-Baroeul,Cambrai,Maubeuge,Lomme,Lambersart,Armentieres,Coudekerque-Branche,Saint-Pol-sur-Mer,La Madeleine,Hazebrouck,Grande-Synthe,Mons-en-Baroeul,Loos,Denain,Croix,Halluin,Hem,Wasquehal,Ronchin,Saint-Amand-les-Eaux,Sin-le-Noble,Faches-Thumesnil,Haubourdin,Hautmont,Bailleul,Wattignies,Anzin,Fourmies,Caudry,Lys-lez-Lannoy,Raismes,Roncq,Mouvaux,Gravelines,Comines,Somain,Seclin,Bruay-sur-l\'Escaut,Marly,Saint-Saulve,Bondues,Jeumont,Saint-Andre-lez-Lille,Marquette-lez-Lille,Vieux-Conde,Annoeullin,Douchy-les-Mines,Conde-sur-l\'Escaut,Aniche','50.568445789474','3.1611184210526'),
                                '60' => array('Beauvais,Compiegne,Creil,Nogent-sur-Oise,Senlis,Crepy-en-Valois,Noyon,Pont-Sainte-Maxence,Meru,Montataire,Chantilly,Clermont','49.314036666667','2.5319425'),
                                '61' => array('Alencon,Argentan,Flers','48.640183333333','-0.16933333333333'),
                                '62' => array('Calais,Arras,Boulogne-sur-Mer,Lens,Lievin,Bethune,Henin-Beaumont,Bruay-la-Buissiere,Carvin,Avion,Saint-Omer,Berck-Plage,Outreau,Berck,Longuenesse,Harnes,Saint-Martin-Boulogne,Bully-les-Mines,Noeux-les-Mines,Mericourt,Auchel,Etaples,Le Portel,Montigny-en-Gohelle,Sallaumines,Oignies,Aire-sur-la-Lys,Courrieres,Lillers,Isbergues','50.532372666667','2.419245'),
                                '63' => array('Clermont-Ferrand,Chamalieres,Riom,Cournon-d\'Auvergne,Issoire,Thiers,Beaumont,Aubiere','45.761685','3.1843575'),
                                '64' => array('Pau,Bayonne,Anglet,Biarritz,Billere,Saint-Jean-de-Luz,Hendaye,Lons,Oloron-Sainte-Marie,Orthez','43.380035','-1.053409'),
                                '65' => array('Tarbes,Lourdes','43.166665','0.016665'),
                                '66' => array('Perpignan,Canet-en-Roussillon,Saint-Esteve,Argelers','42.664135','2.9448775'),
                                '67' => array('Strasbourg,Haguenau,Schiltigheim,Illkirch-Graffenstaden,Selestat,Bischheim,Lingolsheim,Saverne,Bischwiller,Obernai,Ostwald,Hoenheim','48.5926925','7.6709975'),
                                '68' => array('Mulhouse,Colmar,Saint-Louis,Wittenheim,Illzach,Rixheim,Riedisheim,Kingersheim,Guebwiller,Cernay,Wittelsheim','47.803433636364','7.3333736363636'),
                                '69' => array('Lyon,Villeurbanne,Venissieux,Caluire-et-Cuire,Saint-Priest,Bron,Vaulx-en-Velin,Villefranche-sur-Saone,Meyzieu,Rillieux-la-Pape,Oullins,Decines-Charpieu,Sainte-Foy-les-Lyon,Saint-Genis-Laval,Ecully,Givors,Tassin-la-Demi-Lune,Saint-Fons,Genas,Brignais,Francheville,Mions,Tarare','45.749429565217','4.8394686956522'),
                                '70' => array('Vesoul,Hericourt','47.60319','6.46403'));
  
  public static function getDepartements($type='name2code') {
    $out = null;
    if ($type === 'name2code') {
      $out = array();
      foreach (self::$departements as $code => $a) {
        $name = strtolower($a[1]);
        $out[$name] = $code;
      }
      return $out;
    } else if ($type === 'subscribe') {
      $out = array();
      foreach (self::$departements as $code => $a) {
        $name = strtolower($a[1]);
        $out[$code] = $code.' - '.SH::capitalize($name);
      }
      return $out;
    }
  }

  public static function getRegions($type='name2code') {
    $out = null;
    if ($type === 'name2code') {
      $out = array();
      foreach (self::$regions as $code => $name) {
        $name = strtolower($name);
        $out[$name] = $code;
      }
      return $out;
    }
  }

  public static function getRegionByPostalCode($postal_code) {
    $postal_code = str_pad($postal_code, 5, '0', STR_PAD_LEFT);
    $dept = substr($postal_code, 0, 2);
    return self::getRegionByDepartement($dept);
  }

  public static function getRegionByDepartement($dept) {
    $region = null;
    if (is_numeric($dept) || strlen($dept) <= 2) {
      $dept = (string) $dept;
      if (strlen($dept) == 1) {
        $dept = str_pad($dept, 2, '0', STR_PAD_LEFT);
      }
      $dept = self::$departements[$dept];
      $region = $dept[4];
    }
    else {
      $dept = strtolower($dept);
      foreach (self::$departements as $h) {
        if (strtolower($h[1]) == $dept) {
          $region = $h[4];
          break ;
        }
      }
    }
    if (!$region) return null;
    $code = $region;
    $name = self::$regions[$code];
    return array('code' => $code,
                 'name' => $name);
  }

}
