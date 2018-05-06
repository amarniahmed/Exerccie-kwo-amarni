-- MySQL dump 10.13  Distrib 5.6.39, for Linux (x86_64)
--
-- Host: localhost    Database: test
-- ------------------------------------------------------
-- Server version	5.6.39

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `core_abuse`
--

DROP TABLE IF EXISTS `core_abuse`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_abuse` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `content` text NOT NULL,
  `email` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `full_name` varchar(255) NOT NULL,
  `motive_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `seriousness` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `motive_id` (`motive_id`),
  KEY `item` (`model_id`,`record_id`)
) ENGINE=MyISAM AUTO_INCREMENT=63 DEFAULT CHARSET=utf8 COMMENT='report d''abus';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_abuse`
--

LOCK TABLES `core_abuse` WRITE;
/*!40000 ALTER TABLE `core_abuse` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_abuse` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_acl`
--

DROP TABLE IF EXISTS `core_acl`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_acl` (
  `model_id` smallint(5) unsigned NOT NULL,
  `record_id` int(10) unsigned NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `permissions` int(10) unsigned NOT NULL DEFAULT '0',
  `granted_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`model_id`,`record_id`,`user_id`),
  KEY `user` (`user_id`,`model_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_acl`
--

LOCK TABLES `core_acl` WRITE;
/*!40000 ALTER TABLE `core_acl` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_acl` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_act`
--

DROP TABLE IF EXISTS `core_act`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_act` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `email_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `campaign_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `value` int(10) unsigned NOT NULL DEFAULT '0',
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `act` (`campaign_id`,`type`,`email_id`,`value`),
  KEY `type` (`type`)
) ENGINE=MyISAM AUTO_INCREMENT=104 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_act`
--

LOCK TABLES `core_act` WRITE;
/*!40000 ALTER TABLE `core_act` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_act` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_addressee`
--

DROP TABLE IF EXISTS `core_addressee`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_addressee` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `address` varchar(255) NOT NULL,
  `address_extra` varchar(255) NOT NULL,
  `admin2` varchar(16) NOT NULL,
  `city` varchar(255) NOT NULL,
  `civility` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `country_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `first_name` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `information` text NOT NULL COMMENT 'digicode, etc',
  `last_name` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `phone` varchar(32) NOT NULL,
  `postal_code` varchar(32) NOT NULL,
  `region` varchar(128) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM AUTO_INCREMENT=1634 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_addressee`
--

LOCK TABLES `core_addressee` WRITE;
/*!40000 ALTER TABLE `core_addressee` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_addressee` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_admin`
--

DROP TABLE IF EXISTS `core_admin`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_admin` (
  `id` smallint(3) unsigned NOT NULL AUTO_INCREMENT,
  `email` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locale` int(10) unsigned NOT NULL DEFAULT '1',
  `login` varchar(16) NOT NULL,
  `name` varchar(255) NOT NULL,
  `password` varchar(16) NOT NULL,
  `policy_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `root_path` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  UNIQUE KEY `login` (`login`)
) ENGINE=MyISAM AUTO_INCREMENT=8 DEFAULT CHARSET=utf8 PACK_KEYS=0 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_admin`
--

LOCK TABLES `core_admin` WRITE;
/*!40000 ALTER TABLE `core_admin` DISABLE KEYS */;
INSERT INTO `core_admin` VALUES (1,'noc@kernix.com',1,1,'admin','super utilisateur','password',1,'',1);
/*!40000 ALTER TABLE `core_admin` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_advert`
--

DROP TABLE IF EXISTS `core_advert`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_advert` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `advertiser_id` int(10) unsigned NOT NULL DEFAULT '0',
  `app` varchar(16) NOT NULL,
  `description` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `format` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `height` smallint(5) unsigned NOT NULL DEFAULT '0',
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `medium` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `name` varchar(255) NOT NULL,
  `online_from` date NOT NULL DEFAULT '0000-00-00',
  `online_to` date NOT NULL DEFAULT '0000-00-00',
  `profit` float(6,2) unsigned NOT NULL DEFAULT '0.00',
  `source` text NOT NULL,
  `spaces` int(10) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `target_url` varchar(255) NOT NULL,
  `threshold` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `title` varchar(255) NOT NULL,
  `type` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `width` smallint(5) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `advertiser_id` (`advertiser_id`),
  KEY `format` (`format`),
  KEY `range` (`online_from`,`online_to`),
  KEY `app` (`app`)
) ENGINE=MyISAM AUTO_INCREMENT=28 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_advert`
--

LOCK TABLES `core_advert` WRITE;
/*!40000 ALTER TABLE `core_advert` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_advert` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_advert_space`
--

DROP TABLE IF EXISTS `core_advert_space`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_advert_space` (
  `advert_id` int(10) unsigned NOT NULL DEFAULT '0',
  `space_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`advert_id`,`space_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_advert_space`
--

LOCK TABLES `core_advert_space` WRITE;
/*!40000 ALTER TABLE `core_advert_space` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_advert_space` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_advertiser`
--

DROP TABLE IF EXISTS `core_advertiser`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_advertiser` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(16) NOT NULL,
  `content` text NOT NULL,
  `name` varchar(255) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '200',
  `title` varchar(255) NOT NULL,
  `type` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_advertiser`
--

LOCK TABLES `core_advertiser` WRITE;
/*!40000 ALTER TABLE `core_advertiser` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_advertiser` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_affiliate`
--

DROP TABLE IF EXISTS `core_affiliate`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_affiliate` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `cost` float(6,2) unsigned NOT NULL DEFAULT '0.00',
  `description` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_affiliate`
--

LOCK TABLES `core_affiliate` WRITE;
/*!40000 ALTER TABLE `core_affiliate` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_affiliate` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_alert`
--

DROP TABLE IF EXISTS `core_alert`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_alert` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `count` smallint(5) unsigned NOT NULL DEFAULT '0',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `frequency` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `name` varchar(128) NOT NULL,
  `search_id` int(10) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `user_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM AUTO_INCREMENT=127 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_alert`
--

LOCK TABLES `core_alert` WRITE;
/*!40000 ALTER TABLE `core_alert` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_alert` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_alert_queue`
--

DROP TABLE IF EXISTS `core_alert_queue`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_alert_queue` (
  `alert_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `delivery_id` int(10) unsigned NOT NULL DEFAULT '0',
  `user_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `inserted_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `sent_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  KEY `model_id` (`model_id`),
  KEY `user_id` (`user_id`),
  KEY `sended_on` (`sent_at`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_alert_queue`
--

LOCK TABLES `core_alert_queue` WRITE;
/*!40000 ALTER TABLE `core_alert_queue` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_alert_queue` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_announcement`
--

DROP TABLE IF EXISTS `core_announcement`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_announcement` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `content` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `sent_on` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `status` tinyint(4) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `item` (`model_id`,`record_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_announcement`
--

LOCK TABLES `core_announcement` WRITE;
/*!40000 ALTER TABLE `core_announcement` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_announcement` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_appeal`
--

DROP TABLE IF EXISTS `core_appeal`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_appeal` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `appealed_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `content` text NOT NULL,
  `datas` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `value` int(10) unsigned NOT NULL DEFAULT '0',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `item` (`model_id`,`record_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_appeal`
--

LOCK TABLES `core_appeal` WRITE;
/*!40000 ALTER TABLE `core_appeal` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_appeal` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_area`
--

DROP TABLE IF EXISTS `core_area`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_area` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `admins` text NOT NULL,
  `country_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `latitude` double NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `longitude` double NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `radius` double unsigned NOT NULL DEFAULT '0',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=12 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_area`
--

LOCK TABLES `core_area` WRITE;
/*!40000 ALTER TABLE `core_area` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_area` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_auth_attempt`
--

DROP TABLE IF EXISTS `core_auth_attempt`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_auth_attempt` (
  `ip` int(10) unsigned NOT NULL DEFAULT '0',
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `ip` (`ip`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_auth_attempt`
--

LOCK TABLES `core_auth_attempt` WRITE;
/*!40000 ALTER TABLE `core_auth_attempt` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_auth_attempt` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_author`
--

DROP TABLE IF EXISTS `core_author`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_author` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `admin_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `description` text NOT NULL,
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  `website` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM AUTO_INCREMENT=10 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_author`
--

LOCK TABLES `core_author` WRITE;
/*!40000 ALTER TABLE `core_author` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_author` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_badword`
--

DROP TABLE IF EXISTS `core_badword`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_badword` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_badword`
--

LOCK TABLES `core_badword` WRITE;
/*!40000 ALTER TABLE `core_badword` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_badword` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_base`
--

DROP TABLE IF EXISTS `core_base`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_base` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(16) NOT NULL,
  `code` varchar(64) NOT NULL,
  `confirmation` text NOT NULL,
  `description` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `position` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `title` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`,`locale`)
) ENGINE=MyISAM AUTO_INCREMENT=68 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_base`
--

LOCK TABLES `core_base` WRITE;
/*!40000 ALTER TABLE `core_base` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_base` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_base_email`
--

DROP TABLE IF EXISTS `core_base_email`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_base_email` (
  `base_id` smallint(6) NOT NULL DEFAULT '0',
  `email_id` int(11) NOT NULL DEFAULT '0',
  `status` tinyint(1) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`email_id`,`base_id`),
  KEY `base_id` (`base_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_base_email`
--

LOCK TABLES `core_base_email` WRITE;
/*!40000 ALTER TABLE `core_base_email` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_base_email` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_beneficiary`
--

DROP TABLE IF EXISTS `core_beneficiary`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_beneficiary` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `birth_date` date NOT NULL DEFAULT '0000-00-00',
  `civility` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `contact_id` int(10) unsigned NOT NULL DEFAULT '0',
  `email` varchar(255) NOT NULL,
  `first_name` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `last_name` varchar(255) NOT NULL,
  `link` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `mobile` varchar(16) NOT NULL,
  `name` varchar(255) NOT NULL,
  `phone` varchar(16) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_beneficiary`
--

LOCK TABLES `core_beneficiary` WRITE;
/*!40000 ALTER TABLE `core_beneficiary` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_beneficiary` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_blacklist`
--

DROP TABLE IF EXISTS `core_blacklist`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_blacklist` (
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  `culprit_id` int(10) unsigned NOT NULL DEFAULT '0',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `inserted_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`user_id`,`culprit_id`),
  KEY `cause` (`model_id`,`record_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_blacklist`
--

LOCK TABLES `core_blacklist` WRITE;
/*!40000 ALTER TABLE `core_blacklist` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_blacklist` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_block`
--

DROP TABLE IF EXISTS `core_block`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_block` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `about` tinytext NOT NULL,
  `app` varchar(32) NOT NULL,
  `code` varchar(32) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `icon` varchar(255) NOT NULL,
  `image` varchar(255) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `provider` varchar(255) NOT NULL COMMENT 'action associée',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `thumbnail` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=70 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_block`
--

LOCK TABLES `core_block` WRITE;
/*!40000 ALTER TABLE `core_block` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_block` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_block_page`
--

DROP TABLE IF EXISTS `core_block_page`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_block_page` (
  `block_id` int(10) unsigned NOT NULL DEFAULT '0',
  `page_id` int(10) unsigned NOT NULL DEFAULT '0',
  `position` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`page_id`,`block_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_block_page`
--

LOCK TABLES `core_block_page` WRITE;
/*!40000 ALTER TABLE `core_block_page` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_block_page` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_blog`
--

DROP TABLE IF EXISTS `core_blog`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_blog` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(16) NOT NULL,
  `code` varchar(64) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `sections` int(10) unsigned NOT NULL DEFAULT '0',
  `skin` varchar(255) NOT NULL,
  `skin_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `links` text NOT NULL,
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`),
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_blog`
--

LOCK TABLES `core_blog` WRITE;
/*!40000 ALTER TABLE `core_blog` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_blog` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_body`
--

DROP TABLE IF EXISTS `core_body`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_body` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `name` varchar(255) NOT NULL,
  `content_html` mediumtext NOT NULL,
  `content_text` mediumtext NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_body`
--

LOCK TABLES `core_body` WRITE;
/*!40000 ALTER TABLE `core_body` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_body` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_bucket`
--

DROP TABLE IF EXISTS `core_bucket`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_bucket` (
  `stem_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `document_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `weight` tinyint(3) unsigned NOT NULL DEFAULT '0',
  KEY `document_id` (`document_id`),
  KEY `stem_id` (`stem_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_bucket`
--

LOCK TABLES `core_bucket` WRITE;
/*!40000 ALTER TABLE `core_bucket` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_bucket` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_burst`
--

DROP TABLE IF EXISTS `core_burst`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_burst` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `campaign_id` int(10) unsigned NOT NULL DEFAULT '0',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  KEY `campaign_id` (`campaign_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_burst`
--

LOCK TABLES `core_burst` WRITE;
/*!40000 ALTER TABLE `core_burst` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_burst` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_calendar`
--

DROP TABLE IF EXISTS `core_calendar`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_calendar` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `content` text NOT NULL,
  `description` text NOT NULL,
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `thumbnail` varchar(255) NOT NULL,
  `title` varchar(255) NOT NULL,
  `type` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_calendar`
--

LOCK TABLES `core_calendar` WRITE;
/*!40000 ALTER TABLE `core_calendar` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_calendar` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_campaign`
--

DROP TABLE IF EXISTS `core_campaign`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_campaign` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `bases` varchar(255) NOT NULL,
  `blacklist` text NOT NULL,
  `body_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `content_html` mediumtext NOT NULL,
  `content_text` mediumtext NOT NULL,
  `delivered_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `flags` int(11) NOT NULL DEFAULT '0',
  `from_email` varchar(255) NOT NULL,
  `from_name` varchar(255) NOT NULL,
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `newsletter_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `number` smallint(5) unsigned NOT NULL DEFAULT '0',
  `progress` varchar(255) NOT NULL,
  `reply_email` varchar(255) NOT NULL,
  `scheduled_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `style` varchar(500) NOT NULL,
  `subject` varchar(255) NOT NULL,
  `test_bases` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `name` (`name`(2)),
  KEY `subject` (`subject`(2)),
  KEY `item` (`model_id`,`record_id`)
) ENGINE=MyISAM AUTO_INCREMENT=40 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_campaign`
--

LOCK TABLES `core_campaign` WRITE;
/*!40000 ALTER TABLE `core_campaign` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_campaign` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_campaign_queue`
--

DROP TABLE IF EXISTS `core_campaign_queue`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_campaign_queue` (
  `id` int(10) unsigned NOT NULL,
  `email_id` mediumint(8) unsigned NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`,`email_id`),
  KEY `created_at` (`created_at`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_campaign_queue`
--

LOCK TABLES `core_campaign_queue` WRITE;
/*!40000 ALTER TABLE `core_campaign_queue` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_campaign_queue` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_captcha`
--

DROP TABLE IF EXISTS `core_captcha`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_captcha` (
  `client` double NOT NULL DEFAULT '0',
  `token` int(10) unsigned NOT NULL DEFAULT '0',
  `value` varchar(8) NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `ip` (`client`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_captcha`
--

LOCK TABLES `core_captcha` WRITE;
/*!40000 ALTER TABLE `core_captcha` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_captcha` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_carrier`
--

DROP TABLE IF EXISTS `core_carrier`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_carrier` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(16) NOT NULL,
  `countries` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `position` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `priority` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `rates` mediumtext NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `threshold_max` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `threshold_min` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_carrier`
--

LOCK TABLES `core_carrier` WRITE;
/*!40000 ALTER TABLE `core_carrier` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_carrier` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_carrier_rate`
--

DROP TABLE IF EXISTS `core_carrier_rate`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_carrier_rate` (
  `admin2` varchar(7) NOT NULL,
  `amount` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `carrier_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `country_id` smallint(5) unsigned NOT NULL DEFAULT '250',
  `volume_min` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `volume_max` float(9,2) unsigned NOT NULL DEFAULT '100000.00',
  `weight_min` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `weight_max` float(9,2) unsigned NOT NULL DEFAULT '100000.00',
  `amount_min` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `amount_max` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  KEY `country_id` (`country_id`),
  KEY `admin2` (`admin2`),
  KEY `volume` (`volume_min`,`volume_max`),
  KEY `weight` (`weight_min`,`weight_max`),
  KEY `carrier_id` (`carrier_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='tarif de livraison';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_carrier_rate`
--

LOCK TABLES `core_carrier_rate` WRITE;
/*!40000 ALTER TABLE `core_carrier_rate` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_carrier_rate` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_chapter`
--

DROP TABLE IF EXISTS `core_chapter`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_chapter` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `position` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `story_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `story_id` (`story_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_chapter`
--

LOCK TABLES `core_chapter` WRITE;
/*!40000 ALTER TABLE `core_chapter` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_chapter` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_choice`
--

DROP TABLE IF EXISTS `core_choice`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_choice` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `image` varchar(255) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `poll_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `position` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  KEY `poll_id` (`poll_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_choice`
--

LOCK TABLES `core_choice` WRITE;
/*!40000 ALTER TABLE `core_choice` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_choice` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_client_failure`
--

DROP TABLE IF EXISTS `core_client_failure`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_client_failure` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `attempts` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `attempted_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `datas` text NOT NULL,
  `message` text NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `inserted_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_client_failure`
--

LOCK TABLES `core_client_failure` WRITE;
/*!40000 ALTER TABLE `core_client_failure` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_client_failure` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_cluster`
--

DROP TABLE IF EXISTS `core_cluster`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_cluster` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `description` text NOT NULL,
  `dimensions` text NOT NULL,
  `model_id` smallint(6) NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_cluster`
--

LOCK TABLES `core_cluster` WRITE;
/*!40000 ALTER TABLE `core_cluster` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_cluster` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_column`
--

DROP TABLE IF EXISTS `core_column`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_column` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `content` text NOT NULL,
  `description` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `position` smallint(5) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_column`
--

LOCK TABLES `core_column` WRITE;
/*!40000 ALTER TABLE `core_column` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_column` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_comment`
--

DROP TABLE IF EXISTS `core_comment`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_comment` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `model_id` smallint(5) unsigned NOT NULL,
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `content` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `posted_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `item` (`model_id`,`record_id`),
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM AUTO_INCREMENT=448 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_comment`
--

LOCK TABLES `core_comment` WRITE;
/*!40000 ALTER TABLE `core_comment` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_comment` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_component`
--

DROP TABLE IF EXISTS `core_component`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_component` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `faq_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `file` varchar(255) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `position` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `section_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  KEY `faq_id` (`faq_id`),
  KEY `section_id` (`section_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_component`
--

LOCK TABLES `core_component` WRITE;
/*!40000 ALTER TABLE `core_component` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_component` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_contact`
--

DROP TABLE IF EXISTS `core_contact`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_contact` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `address` varchar(255) NOT NULL,
  `address_extra` varchar(255) NOT NULL,
  `birth_date` date NOT NULL DEFAULT '0000-00-00',
  `city` varchar(255) NOT NULL,
  `city_id` int(10) unsigned NOT NULL DEFAULT '0',
  `civility` tinyint(4) NOT NULL DEFAULT '0',
  `code` varchar(64) NOT NULL,
  `country_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `description` text NOT NULL,
  `email` varchar(255) NOT NULL,
  `fax` varchar(64) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `first_name` varchar(255) NOT NULL,
  `latitude` double NOT NULL DEFAULT '0',
  `last_name` varchar(255) NOT NULL,
  `longitude` double NOT NULL DEFAULT '0',
  `mobile` varchar(64) NOT NULL,
  `name` varchar(255) NOT NULL,
  `organization` varchar(255) NOT NULL,
  `occupation` varchar(255) NOT NULL,
  `phone` varchar(64) NOT NULL,
  `postal_code` varchar(64) NOT NULL,
  `region` varchar(64) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `item` (`model_id`,`record_id`)
) ENGINE=MyISAM AUTO_INCREMENT=16 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_contact`
--

LOCK TABLES `core_contact` WRITE;
/*!40000 ALTER TABLE `core_contact` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_contact` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_content`
--

DROP TABLE IF EXISTS `core_content`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_content` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `content` text NOT NULL,
  `crc` int(11) NOT NULL DEFAULT '0' COMMENT 'guid CRC32 ',
  `description` text NOT NULL,
  `feed_id` int(11) NOT NULL DEFAULT '0',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `published_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `item` (`model_id`,`record_id`),
  KEY `published_at` (`published_at`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_content`
--

LOCK TABLES `core_content` WRITE;
/*!40000 ALTER TABLE `core_content` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_content` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_conversion`
--

DROP TABLE IF EXISTS `core_conversion`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_conversion` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `adjudged_at` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `campaign` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `converted_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `cost` float(6,2) unsigned NOT NULL DEFAULT '0.00',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `layout` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'a/b testing',
  `media` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `medium` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `nth_conversion_day` smallint(5) unsigned NOT NULL DEFAULT '1' COMMENT 'nième journée de conversion',
  `nth_visit_day` smallint(5) unsigned NOT NULL DEFAULT '1' COMMENT 'nième journée de visite',
  `source` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `span` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'days since first visit',
  `status` tinyint(1) NOT NULL DEFAULT '1',
  `term` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  `visit_id` int(10) unsigned NOT NULL DEFAULT '0',
  `visit_number` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `visitor_id` int(10) unsigned NOT NULL DEFAULT '0',
  `winner_id` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'visit_id that wins the lead',
  `worth` float(6,2) unsigned NOT NULL DEFAULT '0.00',
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`),
  KEY `visit_id` (`visit_id`),
  KEY `model_id` (`model_id`),
  KEY `medium` (`medium`),
  KEY `converted_at` (`converted_at`)
) ENGINE=MyISAM AUTO_INCREMENT=1919 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_conversion`
--

LOCK TABLES `core_conversion` WRITE;
/*!40000 ALTER TABLE `core_conversion` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_conversion` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_country`
--

DROP TABLE IF EXISTS `core_country`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_country` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `continent_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `code` varchar(2) NOT NULL,
  `iso3_code` varchar(3) NOT NULL,
  `fips_code` varchar(3) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `currency` varchar(3) NOT NULL,
  `name` varchar(255) NOT NULL,
  `name_fr` varchar(255) NOT NULL,
  `name_en` varchar(255) NOT NULL,
  `name_es` varchar(255) NOT NULL,
  `name_de` varchar(255) NOT NULL,
  `name_it` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`),
  UNIQUE KEY `iso3_code` (`iso3_code`),
  KEY `status` (`status`),
  KEY `name` (`name_fr`(2)),
  KEY `fips_code` (`fips_code`)
) ENGINE=MyISAM AUTO_INCREMENT=895 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_country`
--

LOCK TABLES `core_country` WRITE;
/*!40000 ALTER TABLE `core_country` DISABLE KEYS */;
INSERT INTO `core_country` VALUES (4,6255147,0,'AF','AFG','AF',32,'AFA','afghanistan','AFGHANISTAN','AFGHANISTAN','AFGANISTÁN','AFGHANISTAN','AFGANISTÁN'),(8,6255148,1,'AL','ALB','AL',36,'ALL','albanie','ALBANIE','ALBANIA','ALBANIA','ALBANIEN','ALBANIA'),(10,6255152,0,'AQ','ATA','AY',32,'NOK','antarctique','ANTARCTIQUE','ANTARCTICA','ANTÁRTIDA','ANTARKTIS','ANTÁRTIDA'),(12,6255146,1,'DZ','DZA','AG',36,'DZD','algérie','ALGÉRIE','ALGERIA','ARGELIA','ALGERIET','ARGELIA'),(16,6255151,0,'AS','ASM','AQ',32,'USD','samoa américaines','SAMOA AMÉRICAINES','AMERICAN SAMOA','SAMOA AMERICANA','AMERIKANSK SAMOA','SAMOA AMERICANA'),(20,6255148,1,'AD','AND','AN',36,'EUR','andorre','ANDORRE','ANDORRA','ANDORRA','ANDORRA','ANDORRA'),(24,6255146,0,'AO','AGO','AO',32,'AON','angola','ANGOLA','ANGOLA','ANGOLA','ANGOLA','ANGOLA'),(28,6255149,0,'AG','ATG','AC',32,'XCD','antigua-et-barbuda','ANTIGUA-ET-BARBUDA','ANTIGUA AND BARBUDA','ANTIGUA Y BARBUDA','ANTIGUA OG BARBUDA','ANTIGUA Y BARBUDA'),(31,6255147,1,'AZ','AZE','AJ',36,'AZM','azerbaïdjan','AZERBAÏDJAN','AZERBAIJAN','AZERBAIYÁN','ASERBAJDSJAN','AZERBAIYÁN'),(32,6255150,1,'AR','ARG','AR',36,'ARA','argentine','ARGENTINE','ARGENTINA','ARGENTINA','ARGENTINA','ARGENTINA'),(36,6255151,1,'AU','AUS','AS',36,'AUD','australie','AUSTRALIE','AUSTRALIA','AUSTRALIA','AUSTRALIEN','AUSTRALIA'),(40,6255148,1,'AT','AUT','AU',39,'EUR','autriche','AUTRICHE','AUSTRIA','AUSTRIA','ØSTRIG','AUSTRIA'),(44,6255149,1,'BS','BHS','BF',36,'BSD','bahamas','BAHAMAS','BAHAMAS','BAHAMAS','BAHAMAS','BAHAMAS'),(48,6255147,0,'BH','BHR','BA',32,'BHD','bahreïn','BAHREÏN','BAHRAIN','BAHRÁIN','BAHRAIN','BAHRÁIN'),(50,6255147,0,'BD','BGD','BG',32,'BDT','bangladesh','BANGLADESH','BANGLADESH','BANGLADESH','BANGLADESH','BANGLADESH'),(51,6255147,1,'AM','ARM','AM',36,'AMD','arménie','ARMÉNIE','ARMENIA','ARMENIA','ARMENIEN','ARMENIA'),(52,6255149,1,'BB','BRB','BB',36,'BBD','barbade','BARBADE','BARBADOS','BARBADOS','BARBADOS','BARBADOS'),(56,6255148,1,'BE','BEL','BE',39,'EUR','belgique','BELGIQUE','BELGIUM','BÉLGICA','BELGIEN','BÉLGICA'),(60,6255149,1,'BM','BMU','BD',36,'BMD','bermudes','BERMUDES','BERMUDA','BERMUDAS','BERMUDA','BERMUDAS'),(64,6255147,0,'BT','BTN','BT',32,'BTN','bhoutan','BHOUTAN','BHUTAN','BUTÁN','BHUTAN','BUTÁN'),(68,6255150,1,'BO','BOL','BL',36,'BOB','bolivie','BOLIVIE','BOLIVIA','BOLIVIA','BOLIVIA','BOLIVIA'),(70,6255148,0,'BA','BIH','BK',32,'BAM','bosnie-herzégovine','BOSNIE-HERZÉGOVINE','BOSNIA AND HERZEGOVINA','BOSNIA Y HERCEGOVINA','BOSNIEN-HERCEGOVINA','BOSNIA Y HERCEGOVINA'),(72,6255146,0,'BW','BWA','BC',32,'BWP','botswana','BOTSWANA','BOTSWANA','BOTSUANA','BOTSWANA','BOTSUANA'),(74,6255152,0,'BV','BVT','BV',32,'NOK','bouvet, île','BOUVET, ÎLE','BOUVET ISLAND','ISLA BOUVET','BOUVETØ','ISLA BOUVET'),(76,6255150,1,'BR','BRA','BR',36,'BRR','brésil','BRÉSIL','BRAZIL','BRASIL','BRASILIEN','BRASIL'),(84,6255149,0,'BZ','BLZ','BH',32,'BZD','belize','BELIZE','BELIZE','BELICE','BELIZE','BELICE'),(86,6255147,0,'IO','IOT','IO',32,'GBP','océan indien, territoire britannique de l\'','OCÉAN INDIEN, TERRITOIRE BRITANNIQUE DE L\'','BRITISH INDIAN OCEAN TERRITORY','TERRITORIO BRITÁNICO DEL OCÉANO INDICO','DET BRITISKE TERRITORIUM I DET INDISKE OCEAN','TERRITORIO BRITÁNICO DEL OCÉANO INDICO'),(90,6255151,0,'SB','SLB','BP',32,'SBD','salomon, îles','SALOMON, ÎLES','SOLOMON ISLANDS','ISLAS SALOMÓN','SALOMONØERNE','ISLAS SALOMÓN'),(92,6255149,0,'VG','VGB','VI',32,'USD','îles vierges britanniques','ÎLES VIERGES BRITANNIQUES','VIRGIN ISLANDS, BRITISH','ISLAS VÍRGENES BRITÁNICAS','DE BRITISKE JOMFRUØER','ISLAS VÍRGENES BRITÁNICAS'),(96,6255147,0,'BN','BRN','BX',32,'BND','brunéi darussalam','BRUNÉI DARUSSALAM','BRUNEI DARUSSALAM','BRUNÉI','BRUNEI','BRUNÉI'),(100,6255148,1,'BG','BGR','BU',39,'BGL','bulgarie','BULGARIE','BULGARIA','BULGARIA','BULGARIEN','BULGARIA'),(104,6255147,0,'MM','MMR','BM',32,'MMK','myanmar','MYANMAR','MYANMAR','BIRMANIA; MYANMAR','MYANMAR','BIRMANIA; MYANMAR'),(108,6255146,0,'BI','BDI','BY',32,'BIF','burundi','BURUNDI','BURUNDI','BURUNDI','BURUNDI','BURUNDI'),(112,6255148,1,'BY','BLR','BO',36,'BYR','bélarus','BÉLARUS','BELARUS','BIELORRUSIA','BELARUS','BIELORRUSIA'),(116,6255147,1,'KH','KHM','CB',36,'KHR','cambodge','CAMBODGE','CAMBODIA','CAMBOYA','CAMBODJA','CAMBOYA'),(120,6255146,1,'CM','CMR','CM',36,'XAF','cameroun','CAMEROUN','CAMEROON','CAMERÚN','CAMEROUN','CAMERÚN'),(124,6255149,1,'CA','CAN','CA',36,'CAD','canada','CANADA','CANADA','CANADÁ','CANADA','CANADÁ'),(132,6255146,0,'CV','CPV','CV',32,'CVE','cap-vert','CAP-VERT','CAPE VERDE','CABO VERDE','KAP VERDE','CABO VERDE'),(136,6255149,0,'KY','CYM','CJ',32,'KYD','caïmanes, îles','CAÏMANES, ÎLES','CAYMAN ISLANDS','ISLAS CAIMÁN','CAYMANØERNE','ISLAS CAIMÁN'),(140,6255146,0,'CF','CAF','CT',32,'XAF','centrafricaine, république','CENTRAFRICAINE, RÉPUBLIQUE','CENTRAL AFRICAN REPUBLIC','REPÚBLICA CENTROAFRICANA','DEN CENTRALAFRIKANSKE REPUBLIK','REPÚBLICA CENTROAFRICANA'),(144,6255147,1,'LK','LKA','CE',36,'LKR','sri lanka','SRI LANKA','SRI LANKA','SRI LANKA','SRI LANKA','SRI LANKA'),(148,6255146,1,'TD','TCD','CD',36,'XAF','tchad','TCHAD','CHAD','CHAD','TCHAD','CHAD'),(152,6255150,1,'CL','CHL','CI',36,'CLF','chili','CHILI','CHILE','CHILE','CHILE','CHILE'),(156,6255147,1,'CN','CHN','CH',36,'CNY','chine','CHINE','CHINA','CHINA','KINA','CHINA'),(158,6255147,1,'TW','TWN','TW',36,'TWD','taïwan','TAÏWAN','TAIWAN','TAIWÁN','TAIWAN','TAIWÁN'),(162,6255147,0,'CX','CXR','KT',32,'AUD','christmas, île','CHRISTMAS, ÎLE','CHRISTMAS ISLAND','ISLA CHRISTMAS','JULEØEN','ISLA CHRISTMAS'),(166,6255147,0,'CC','CCK','CK',32,'AUD','cocos (keeling), îles','COCOS (KEELING), ÎLES','COCOS (KEELING) ISLANDS','ISLAS COCOS','COCOSØERNE (KEELINGØERNE)','ISLAS COCOS'),(170,6255150,1,'CO','COL','CO',36,'COP','colombie','COLOMBIE','COLOMBIA','COLOMBIA','COLOMBIA','COLOMBIA'),(174,6255146,0,'KM','COM','CN',32,'KMF','comores','COMORES','COMOROS','COMORAS','COMORERNE','COMORAS'),(175,6255146,1,'YT','MYT','MF',47,'EUR','MAYOTTE','MAYOTTE','MAYOTTE','MAYOTTE','MAYOTTE','MAYOTTE'),(178,6255146,1,'CG','COG','CF',36,'XAF','congo','CONGO','CONGO','CONGO','CONGO','CONGO'),(180,6255146,0,'CD','COD','CG',32,'CDZ','congo, la république démocratique du','CONGO, LA RÉPUBLIQUE DÉMOCRATIQUE DU','CONGO, THE DEMOCRATIC REPUBLIC OF THE','REPÚBLICA DEMOCRÁTICA DEL CONGO','CONGO, THE DEMOCRATIC REPUBLIC OF THE','REPÚBLICA DEMOCRÁTICA DEL CONGO'),(184,6255151,0,'CK','COK','CW',32,'NZD','cook, îles','COOK, ÎLES','COOK ISLANDS','ISLAS COOK','COOKØERNE','ISLAS COOK'),(188,6255149,1,'CR','CRI','CS',36,'CRC','costa rica','COSTA RICA','COSTA RICA','COSTA RICA','COSTA RICA','COSTA RICA'),(191,6255148,1,'HR','HRV','HR',36,'HRK','croatie','CROATIE','CROATIA','CROACIA','KROATIEN','CROACIA'),(192,6255149,1,'CU','CUB','CU',36,'CUP','cuba','CUBA','CUBA','CUBA','CUBA','CUBA'),(196,6255148,1,'CY','CYP','CY',39,'CYP','chypre','CHYPRE','CYPRUS','CHIPRE','CYPERN','CHIPRE'),(203,6255148,1,'CZ','CZE','EZ',39,'CZK','tchèque, rép','TCHÈQUE, RÉP','CZECH REPUBLIC','REPÚBLICA CHECA; CHEQUIA','TJEKKIET','REPÚBLICA CHECA; CHEQUIA'),(204,6255146,1,'BJ','BEN','BN',36,'XAF','bénin','BÉNIN','BENIN','BENÍN','BENIN','BENÍN'),(208,6255148,1,'DK','DNK','DA',39,'DKK','danemark','DANEMARK','DENMARK','DINAMARCA','DANMARK','DINAMARCA'),(212,6255149,0,'DM','DMA','DO',32,'XCD','dominique','DOMINIQUE','DOMINICA','DOMINICA','DOMINICA','DOMINICA'),(214,6255149,1,'DO','DOM','DR',36,'DOP','dominicaine, rép','DOMINICAINE, RÉP','DOMINICAN REP','REPÚBLICA DOMINICANA','DEN DOMINIKANSKE REPUBLIK','REPÚBLICA DOMINICANA'),(218,6255150,1,'EC','ECU','EC',36,'USD','équateur','ÉQUATEUR','ECUADOR','ECUADOR','ECUADOR','ECUADOR'),(222,6255149,1,'SV','SLV','ES',36,'USD','el salvador','EL SALVADOR','EL SALVADOR','EL SALVADOR','EL SALVADOR','EL SALVADOR'),(226,6255146,0,'GQ','GNQ','EK',32,'XAF','guinée équatoriale','GUINÉE ÉQUATORIALE','EQUATORIAL GUINEA','GUINEA ECUATORIAL','ÆKVATORIALGUINEA','GUINEA ECUATORIAL'),(231,6255146,0,'ET','ETH','ET',32,'ETB','éthiopie','ÉTHIOPIE','ETHIOPIA','ETIOPÍA','ETIOPIEN','ETIOPÍA'),(232,6255146,0,'ER','ERI','ER',32,'ERN','érythrée','ÉRYTHRÉE','ERITREA','ERITREA','ERITREA','ERITREA'),(233,6255148,1,'EE','EST','EN',39,'EEK','estonie','ESTONIE','ESTONIA','ESTONIA','ESTLAND','ESTONIA'),(234,6255148,0,'FO','FRO','FO',32,'DKK','féroé, îles','FÉROÉ, ÎLES','FAROE ISLANDS','ISLAS FEROE','FÆRØERNE (FØROYAR)','ISLAS FEROE'),(238,6255150,0,'FK','FLK','FK',32,'FKP','ÎLES MALOUINES','ÎLES MALOUINES','FALKLAND ISLANDS (MALVINAS)','ISLAS MALVINAS','FALKLANDSØERNE','ISLAS MALVINAS'),(239,6255152,0,'GS','SGS','SX',32,'GBP','géorgie du sud et les îles sandwich du sud','GÉORGIE DU SUD ET LES ÎLES SANDWICH DU SUD','SOUTH GEORGIA AND THE SOUTH SANDWICH ISLANDS','ISLAS GEORGIA DEL SUR Y SANDWICH DEL SUR','SOUTH GEORGIA OG DE SYDLIGE SANDWICHØER','ISLAS GEORGIA DEL SUR Y SANDWICH DEL SUR'),(242,6255151,1,'FJ','FJI','FJ',36,'FJD','fidji','FIDJI','FIJI','FIYI','FIJI','FIYI'),(246,6255148,1,'FI','FIN','FI',39,'EUR','finlande','FINLANDE','FINLAND','FINLANDIA','FINLAND','FINLANDIA'),(248,6255148,0,'AX','ALA','AL',32,'USD','åland, îles','ÅLAND, ÎLES','ÅLAND ISLANDS','ÅLAND ISLANDS','ÅLAND ISLANDS','ÅLAND ISLANDS'),(250,6255148,1,'FR','FRA','FR',3,'EUR','FRANCE','FRANCE','FRANCE','FRANCIA','FRANKRIG','FRANCIA'),(254,6255150,1,'GF','GUF','FG',47,'EUR','GUYANE FRANÇAISE','GUYANE FRANÇAISE','FRENCH GUIANA','GUAYANA FRANCESA','FRANSK GUYANA','GUAYANA FRANCESA'),(258,6255151,1,'PF','PYF','FP',55,'XPF','POLYNÉSIE FRANÇAISE','POLYNÉSIE FRANÇAISE','FRENCH POLYNESIA','POLINESIA FRANCESA','FRANSK POLYNESIEN','POLINESIA FRANCESA'),(260,6255152,0,'TF','ATF','FS',32,'EUR','terres australes françaises','TERRES AUSTRALES FRANÇAISES','FRENCH SOUTHERN TERRITORIES','TERRITORIOS AUSTRALES FRANCESES','DE FRANSKE BESIDDELSER I DET SYDLIGE INDISKE OCEAN','TERRITORIOS AUSTRALES FRANCESES'),(262,6255146,0,'DJ','DJI','DJ',32,'DJF','djibouti','DJIBOUTI','DJIBOUTI','YIBUTI','DJIBOUTI','YIBUTI'),(266,6255146,1,'GA','GAB','GB',36,'XAF','gabon','GABON','GABON','GABÓN','GABON','GABÓN'),(268,6255147,1,'GE','GEO','GG',36,'GEL','géorgie','GÉORGIE','GEORGIA','GEORGIA','GEORGIEN','GEORGIA'),(270,6255146,0,'GM','GMB','GA',32,'GMD','gambie','GAMBIE','GAMBIA','GAMBIA','GAMBIA','GAMBIA'),(275,6255147,0,'PS','PSE','GZ',32,'USD','palestine','PALESTINE','PALESTINIAN TERRITORY, OCCUPIED','PALESTINIAN TERRITORY, OCCUPIED','PALESTINIAN TERRITORY, OCCUPIED','PALESTINIAN TERRITORY, OCCUPIED'),(276,6255148,1,'DE','DEU','GM',39,'EUR','allemagne','ALLEMAGNE','GERMANY','ALEMANIA','TYSKLAND','ALEMANIA'),(288,6255146,0,'GH','GHA','GH',32,'GHC','ghana','GHANA','GHANA','GHANA','GHANA','GHANA'),(292,6255148,1,'GI','GIB','GI',36,'GIP','gibraltar','GIBRALTAR','GIBRALTAR','GIBRALTAR','GIBRALTAR','GIBRALTAR'),(296,6255151,0,'KI','KIR','KR',32,'AUD','kiribati','KIRIBATI','KIRIBATI','KIRIBATI','KIRIBATI','KIRIBATI'),(300,6255148,1,'GR','GRC','GR',39,'EUR','grèce','GRÈCE','GREECE','GRECIA','GRÆKENLAND','GRECIA'),(304,6255149,1,'GL','GRL','GL',36,'DKK','groenland','GROENLAND','GREENLAND','GROENLANDIA','GRØNLAND (KALAALLIT NUNAAT)','GROENLANDIA'),(308,6255149,1,'GD','GRD','GJ',36,'XCD','grenade','GRENADE','GRENADA','GRANADA','GRENADA','GRANADA'),(312,6255149,1,'GP','GLP','GP',47,'EUR','GUADELOUPE','GUADELOUPE','GUADELOUPE','GUADALUPE','GUADELOUPE','GUADALUPE'),(316,6255151,0,'GU','GUM','GQ',32,'USD','guam','GUAM','GUAM','GUAM','GUAM','GUAM'),(320,6255149,1,'GT','GTM','GT',36,'GTQ','guatemala','GUATEMALA','GUATEMALA','GUATEMALA','GUATEMALA','GUATEMALA'),(324,6255146,1,'GN','GIN','GV',36,'GNS','guinée','GUINÉE','GUINEA','GUINEA','GUINEA','GUINEA'),(328,6255150,0,'GY','GUY','GY',32,'GYD','guyana','GUYANA','GUYANA','GUYANA','GUYANA','GUYANA'),(332,6255149,1,'HT','HTI','HA',36,'HTG','haïti','HAÏTI','HAITI','HAITÍ','HAITI','HAITÍ'),(334,6255152,0,'HM','HMD','HM',32,'AUD','heard, île et mcdonald, îles','HEARD, ÎLE ET MCDONALD, ÎLES','HEARD ISLAND AND MCDONALD ISLANDS','ISLAS HEARD Y MCDONALD','HEARD- OG MCDONALDØERNE','ISLAS HEARD Y MCDONALD'),(336,6255148,0,'VA','VAT','VT',32,'EUR','saint-siège (état de la cité du vatican)','SAINT-SIÈGE (ÉTAT DE LA CITÉ DU VATICAN)','HOLY SEE (VATICAN CITY STATE)','EL VATICANO','VATIKANSTATEN','EL VATICANO'),(340,6255149,0,'HN','HND','HO',32,'HNL','honduras','HONDURAS','HONDURAS','HONDURAS','HONDURAS','HONDURAS'),(344,6255147,1,'HK','HKG','HK',36,'HKD','hong-kong','HONG-KONG','HONG KONG','HONG KONG','HONGKONG','HONG KONG'),(348,6255148,1,'HU','HUN','HU',39,'HUF','hongrie','HONGRIE','HUNGARY','HUNGRÍA','UNGARN','HUNGRÍA'),(352,6255148,1,'IS','ISL','IC',36,'ISK','islande','ISLANDE','ICELAND','ISLANDIA','ISLAND','ISLANDIA'),(356,6255147,1,'IN','IND','IN',36,'INR','inde','INDE','INDIA','INDIA (LA)','INDIEN','INDIA (LA)'),(360,6255147,1,'ID','IDN','ID',36,'IDR','indonésie','INDONÉSIE','INDONESIA','INDONESIA','INDONESIEN','INDONESIA'),(364,6255147,1,'IR','IRN','IR',36,'IRR','iran','IRAN','IRAN','IRÁN','IRAN','IRÁN'),(368,6255147,1,'IQ','IRQ','IZ',36,'IQD','iraq','IRAQ','IRAQ','IRAQ','IRAK','IRAQ'),(372,6255148,1,'IE','IRL','EI',39,'EUR','irlande','IRLANDE','IRELAND','IRLANDA','IRLAND','IRLANDA'),(376,6255147,1,'IL','ISR','IS',36,'ILS','israël','ISRAËL','ISRAEL','ISRAEL','ISRAEL','ISRAEL'),(380,6255148,1,'IT','ITA','IT',39,'EUR','italie','ITALIE','ITALY','ITALIA','ITALIEN','ITALIA'),(384,6255146,1,'CI','CIV','IV',36,'XAF','côte d\'ivoire','CÔTE D\'IVOIRE','COTE D\'IVOIRE','COSTA DE MARFIL','CÔTE D\'IVOIRE','COSTA DE MARFIL'),(388,6255149,1,'JM','JAM','JM',36,'JMD','jamaïque','JAMAÏQUE','JAMAICA','JAMAICA','JAMAICA','JAMAICA'),(392,6255147,1,'JP','JPN','JA',36,'JPY','japon','JAPON','JAPAN','JAPÓN','JAPAN','JAPÓN'),(398,6255147,1,'KZ','KAZ','KZ',36,'KZT','kazakhstan','KAZAKHSTAN','KAZAKHSTAN','KAZAJISTÁN','KASAKHSTAN','KAZAJISTÁN'),(400,6255147,1,'JO','JOR','JO',36,'JOD','jordanie','JORDANIE','JORDAN','JORDANIA','JORDAN','JORDANIA'),(404,6255146,1,'KE','KEN','KE',36,'KES','kenya','KENYA','KENYA','KENIA','KENYA','KENIA'),(408,6255147,0,'KP','PRK','KN',32,'KPW','corée, république populaire démocratique de','CORÉE, RÉPUBLIQUE POPULAIRE DÉMOCRATIQUE DE','KOREA, DEMOCRATIC PEOPLE\'S REPUBLIC OF','COREA DEL NORTE','NORDKOREA','COREA DEL NORTE'),(410,6255147,1,'KR','KOR','KS',36,'KRW','corée, rép de','CORÉE, RÉP DE','KOREA, REP OF','COREA DEL SUR','SYDKOREA','COREA DEL SUR'),(414,6255147,1,'KW','KWT','KU',36,'KWD','koweït','KOWEÏT','KUWAIT','KUWAIT','KUWAIT','KUWAIT'),(417,6255147,0,'KG','KGZ','KG',32,'KGS','kirghizistan','KIRGHIZISTAN','KYRGYZSTAN','KIRGUIZISTÁN','KIRGISISTAN','KIRGUIZISTÁN'),(418,6255147,0,'LA','LAO','LA',32,'LAK','lao, république démocratique populaire','LAO, RÉPUBLIQUE DÉMOCRATIQUE POPULAIRE','LAO PEOPLE\'S DEMOCRATIC REPUBLIC','LAOS','LAOS','LAOS'),(422,6255147,1,'LB','LBN','LE',36,'LBP','liban','LIBAN','LEBANON','LÍBANO','LIBANON','LÍBANO'),(426,6255146,0,'LS','LSO','LT',32,'LSL','lesotho','LESOTHO','LESOTHO','LESOTO','LESOTHO','LESOTO'),(428,6255148,1,'LV','LVA','LG',39,'LVL','lettonie','LETTONIE','LATVIA','LETONIA','LETLAND','LETONIA'),(430,6255146,0,'LR','LBR','LI',32,'LRD','libéria','LIBÉRIA','LIBERIA','LIBERIA','LIBERIA','LIBERIA'),(434,6255146,0,'LY','LBY','LY',32,'LYD','libyenne, jamahiriya arabe','LIBYENNE, JAMAHIRIYA ARABE','LIBYAN ARAB JAMAHIRIYA','LIBIA','LIBYEN','LIBIA'),(438,6255148,1,'LI','LIE','LS',36,'CHF','liechtenstein','LIECHTENSTEIN','LIECHTENSTEIN','LIECHTENSTEIN','LIECHTENSTEIN','LIECHTENSTEIN'),(440,6255148,1,'LT','LTU','LH',39,'LTL','lituanie','LITUANIE','LITHUANIA','LITUANIA','LITAUEN','LITUANIA'),(442,6255148,1,'LU','LUX','LU',39,'EUR','luxembourg','LUXEMBOURG','LUXEMBOURG','LUXEMBURGO','LUXEMBOURG','LUXEMBURGO'),(446,6255147,1,'MO','MAC','MC',36,'MOP','macao','MACAO','MACAO','MACAO','MACAO','MACAO'),(450,6255146,1,'MG','MDG','MA',36,'MGF','madagascar','MADAGASCAR','MADAGASCAR','MADAGASCAR','MADAGASKAR','MADAGASCAR'),(454,6255146,1,'MW','MWI','MI',36,'MWK','malawi','MALAWI','MALAWI','MALAUI','MALAWI','MALAUI'),(458,6255147,1,'MY','MYS','MY',36,'MYR','malaisie','MALAISIE','MALAYSIA','MALASIA','MALAYSIA','MALASIA'),(462,6255147,1,'MV','MDV','MV',36,'MVR','maldives','MALDIVES','MALDIVES','MALDIVAS','MALDIVERNE','MALDIVAS'),(466,6255146,1,'ML','MLI','ML',36,'XAF','mali','MALI','MALI','MALÍ','MALI','MALÍ'),(470,6255148,1,'MT','MLT','MT',39,'MTL','malte','MALTE','MALTA','MALTA','MALTA','MALTA'),(474,6255149,1,'MQ','MTQ','MB',47,'EUR','MARTINIQUE','MARTINIQUE','MARTINIQUE','MARTINICA','MARTINIQUE','MARTINICA'),(478,6255146,1,'MR','MRT','MR',36,'MRO','mauritanie','MAURITANIE','MAURITANIA','MAURITANIA','MAURETANIEN','MAURITANIA'),(480,6255146,1,'MU','MUS','MP',36,'MUR','maurice','MAURICE','MAURITIUS','MAURICIO','MAURITIUS','MAURICIO'),(484,6255149,1,'MX','MEX','MX',36,'MXN','mexique','MEXIQUE','MEXICO','MÉXICO','MEXICO','MÉXICO'),(492,6255148,1,'MC','MCO','MN',36,'EUR','monaco','MONACO','MONACO','MÓNACO','MONACO','MÓNACO'),(496,6255147,1,'MN','MNG','MG',36,'MNT','mongolie','MONGOLIE','MONGOLIA','MONGOLIA','MONGOLIET','MONGOLIA'),(498,6255148,0,'MD','MDA','MD',32,'MDL','moldova, république de','MOLDOVA, RÉPUBLIQUE DE','MOLDOVA, REPUBLIC OF','MOLDAVIA','MOLDOVA','MOLDAVIA'),(499,6255148,1,'ME','MNE','MJ',36,'EUR','monténégro','MONTÉNÉGRO','MONTENEGRO','MONTENEGRO','MONTENEGRO','MONTENEGRO'),(500,6255149,0,'MS','MSR','MH',32,'XCD','montserrat','MONTSERRAT','MONTSERRAT','MONTSERRAT','MONTSERRAT','MONTSERRAT'),(504,6255146,1,'MA','MAR','MO',36,'MAD','maroc','MAROC','MOROCCO','MARRUECOS','MAROKKO','MARRUECOS'),(508,6255146,1,'MZ','MOZ','MZ',36,'MZM','mozambique','MOZAMBIQUE','MOZAMBIQUE','MOZAMBIQUE','MOZAMBIQUE','MOZAMBIQUE'),(512,6255147,0,'OM','OMN','MU',32,'OMR','oman','OMAN','OMAN','OMÁN','OMAN','OMÁN'),(516,6255146,1,'NA','NAM','WA',36,'NAD','namibie','NAMIBIE','NAMIBIA','NAMIBIA','NAMIBIA','NAMIBIA'),(520,6255151,0,'NR','NRU','NR',32,'AUD','nauru','NAURU','NAURU','NAURU','NAURU','NAURU'),(524,6255147,1,'NP','NPL','NP',36,'NPR','népal','NÉPAL','NEPAL','NEPAL','NEPAL','NEPAL'),(528,6255148,1,'NL','NLD','NL',39,'EUR','pays-bas','PAYS-BAS','NETHERLANDS','PAÍSES BAJOS','NEDERLANDENE','PAÍSES BAJOS'),(530,6255149,0,'AN','ANT','NT',32,'ANG','antilles néerlandaises','ANTILLES NÉERLANDAISES','NETHERLANDS ANTILLES','ANTILLAS NEERLANDESAS','DE NEDERLANDSKE ANTILLER','ANTILLAS NEERLANDESAS'),(533,6255149,0,'AW','ABW','AA',32,'AWG','aruba','ARUBA','ARUBA','ARUBA','ARUBA','ARUBA'),(540,6255151,1,'NC','NCL','NC',36,'XPF','nlle-calédonie','NLLE-CALÉDONIE','NEW CALEDONIA','NUEVA CALEDONIA','NY KALEDONIEN','NUEVA CALEDONIA'),(548,6255151,0,'VU','VUT','NH',32,'VUV','vanuatu','VANUATU','VANUATU','VANUATU','VANUATU','VANUATU'),(554,6255151,1,'NZ','NZL','NZ',36,'NZD','nlle-zélande','NLLE-ZÉLANDE','NEW ZEALAND','NUEVA ZELANDA','NEW ZEALAND','NUEVA ZELANDA'),(558,6255149,1,'NI','NIC','NU',36,'NIC','nicaragua','NICARAGUA','NICARAGUA','NICARAGUA','NICARAGUA','NICARAGUA'),(562,6255146,1,'NE','NER','NG',36,'XAF','niger','NIGER','NIGER','NÍGER','NIGER','NÍGER'),(566,6255146,1,'NG','NGA','NI',36,'NGN','nigéria','NIGÉRIA','NIGERIA','NIGERIA','NIGERIA','NIGERIA'),(570,6255151,1,'NU','NIU','NE',36,'NZD','niué','NIUÉ','NIUE','NIUE','NIUE','NIUE'),(574,6255151,1,'NF','NFK','NF',36,'AUD','norfolk, île','NORFOLK, ÎLE','NORFOLK ISLAND','ISLA NORFOLK','NORFOLK ISLAND','ISLA NORFOLK'),(578,6255148,1,'NO','NOR','NO',36,'NOK','norvège','NORVÈGE','NORWAY','NORUEGA','NORGE','NORUEGA'),(580,6255151,0,'MP','MNP','CQ',32,'USD','mariannes du nord, îles','MARIANNES DU NORD, ÎLES','NORTHERN MARIANA ISLANDS','ISLAS MARIANAS DEL NORTE','NORDMARIANERNE','ISLAS MARIANAS DEL NORTE'),(581,6255151,0,'UM','UMI','',32,'USD','îles mineures éloignées des états-unis','ÎLES MINEURES ÉLOIGNÉES DES ÉTATS-UNIS','UNITED STATES MINOR OUTLYING ISLANDS','ISLAS MENORES ALEJADAS DE LOS ESTADOS UNIDOS','DE MINDRE AMERIKANSKE OVERSØISKE ØER','ISLAS MENORES ALEJADAS DE LOS ESTADOS UNIDOS'),(583,6255151,0,'FM','FSM','FM',32,'USD','micronésie, états fédérés de','MICRONÉSIE, ÉTATS FÉDÉRÉS DE','MICRONESIA, FEDERATED STATES OF','MICRONESIA','MIKRONESIEN','MICRONESIA'),(584,6255151,0,'MH','MHL','RM',32,'USD','marshall, îles','MARSHALL, ÎLES','MARSHALL ISLANDS','ISLAS MARSHALL','MARSHALLØERNE','ISLAS MARSHALL'),(585,6255151,0,'PW','PLW','PS',32,'USD','palaos','PALAOS','PALAU','PALAOS','PALAU','PALAOS'),(586,6255147,1,'PK','PAK','PK',36,'PKR','pakistan','PAKISTAN','PAKISTAN','PAKISTÁN','PAKISTAN','PAKISTÁN'),(591,6255149,1,'PA','PAN','PM',36,'USD','panama','PANAMA','PANAMA','PANAMÁ','PANAMA','PANAMÁ'),(598,6255151,0,'PG','PNG','PP',32,'PGK','papouasie-nouvelle-guinée','PAPOUASIE-NOUVELLE-GUINÉE','PAPUA NEW GUINEA','PAPÚA-NUEVA GUINEA','PAPUA NY GUINEA','PAPÚA-NUEVA GUINEA'),(600,6255150,1,'PY','PRY','PA',36,'PYG','paraguay','PARAGUAY','PARAGUAY','PARAGUAY','PARAGUAY','PARAGUAY'),(604,6255150,1,'PE','PER','PE',36,'PEI','pérou','PÉROU','PERU','PERÚ','PERU','PERÚ'),(608,6255147,1,'PH','PHL','RP',36,'PHP','philippines','PHILIPPINES','PHILIPPINES','FILIPINAS','FILIPPINERNE','FILIPINAS'),(612,6255151,0,'PN','PCN','PC',32,'NZD','pitcairn','PITCAIRN','PITCAIRN','ISLAS PITCAIRN','PITCAIRN','ISLAS PITCAIRN'),(616,6255148,1,'PL','POL','PL',39,'PLN','pologne','POLOGNE','POLAND','POLONIA','POLEN','POLONIA'),(620,6255148,1,'PT','PRT','PO',39,'EUR','portugal','PORTUGAL','PORTUGAL','PORTUGAL','PORTUGAL','PORTUGAL'),(624,6255146,1,'GW','GNB','PU',36,'GWP','guinée-bissau','GUINÉE-BISSAU','GUINEA-BISSAU','GUINEA-BISSAU','GUINEA-BISSAU','GUINEA-BISSAU'),(626,6255151,0,'TL','TLS','TT',32,'USD','timor-leste','TIMOR-LESTE','TIMOR-LESTE','TIMOR ORIENTAL','ØSTTIMOR','TIMOR ORIENTAL'),(630,6255149,1,'PR','PRI','RQ',36,'USD','porto rico','PORTO RICO','PUERTO RICO','PUERTO RICO','PUERTO RICO','PUERTO RICO'),(634,6255147,1,'QA','QAT','QA',36,'QAR','qatar','QATAR','QATAR','QATAR','QATAR','QATAR'),(638,6255146,1,'RE','REU','RE',47,'EUR','RÉUNION','RÉUNION','REUNION','REUNIÓN','RÉUNION','REUNIÓN'),(642,6255148,1,'RO','ROU','RO',39,'ROL','roumanie','ROUMANIE','ROMANIA','RUMANIA; RUMANÍA','RUMÆNIEN','RUMANIA; RUMANÍA'),(643,6255148,1,'RU','RUS','RS',36,'RUB','russie, féd','RUSSIE, FÉD','RUSSIAN FED','RUSIA','RUSLAND','RUSIA'),(646,6255146,0,'RW','RWA','RW',32,'RWF','rwanda','RWANDA','RWANDA','RUANDA','RWANDA','RUANDA'),(652,6255149,1,'BL','BLM','TM',55,'EUR','SAINT BARTHÉLEMY','SAINT BARTHÉLEMY','SAINT BARTHELEMY','SAINT BARTHELEMY','SAINT BARTHELEMY','SAINT BARTHELEMY'),(654,6255146,0,'SH','SHN','SH',32,'SHP','sainte-hélène','SAINTE-HÉLÈNE','SAINT HELENA','SANTA HELENA','SAINT HELENA','SANTA HELENA'),(659,6255149,0,'KN','KNA','SC',32,'XCD','saint-kitts-et-nevis','SAINT-KITTS-ET-NEVIS','SAINT KITTS AND NEVIS','SAN CRISTÓBAL Y NIEVES','SAINT KITTS OG NEVIS','SAN CRISTÓBAL Y NIEVES'),(660,6255149,0,'AI','AIA','AV',32,'XCD','anguilla','ANGUILLA','ANGUILLA','ANGUILA','ANGUILLA','ANGUILA'),(662,6255149,1,'LC','LCA','ST',36,'XCD','sainte-lucie','SAINTE-LUCIE','SAINT LUCIA','SANTA LUCÍA','SAINT LUCIA','SANTA LUCÍA'),(663,6255149,1,'MF','MAF','RN',55,'EUR','SAINT MARTIN','SAINT MARTIN','SAINT MARTIN','SAINT MARTIN','SAINT MARTIN','SAINT MARTIN'),(666,6255149,1,'PM','SPM','SB',51,'EUR','SAINT-PIERRE-ET-MIQUELON','SAINT-PIERRE-ET-MIQUELON','SAINT PIERRE AND MIQUELON','SAN PEDRO Y MIQUELÓN','SAINT PIERRE OG MIQUELON','SAN PEDRO Y MIQUELÓN'),(670,6255149,0,'VC','VCT','VC',32,'XCD','saint-vincent-et-les grenadines','SAINT-VINCENT-ET-LES GRENADINES','SAINT VINCENT AND THE GRENADINES','SAN VICENTE Y LAS GRANADINAS','SAINT VINCENT OG GRENADINERNE','SAN VICENTE Y LAS GRANADINAS'),(674,6255148,1,'SM','SMR','SM',36,'EUR','saint-marin','SAINT-MARIN','SAN MARINO','SAN MARINO','SAN MARINO','SAN MARINO'),(678,6255146,0,'ST','STP','TP',32,'STD','sao tomé-et-principe','SAO TOMÉ-ET-PRINCIPE','SAO TOME AND PRINCIPE','SANTO TOMÉ Y PRÍNCIPE','SÃO TOMÉ OG PRÍNCIPE','SANTO TOMÉ Y PRÍNCIPE'),(682,6255147,1,'SA','SAU','SA',36,'SAR','arabie saoudite','ARABIE SAOUDITE','SAUDI ARABIA','ARABIA SAUDÍ','SAUDI-ARABIEN','ARABIA SAUDÍ'),(686,6255146,1,'SN','SEN','SG',36,'XOF','sénégal','SÉNÉGAL','SENEGAL','SENEGAL','SENEGAL','SENEGAL'),(688,6255148,1,'RS','SRB','RB',36,'RSD','serbie','SERBIE','SERBIA','SERBIA','SERBIA','SERBIA'),(690,6255146,0,'SC','SYC','SE',32,'SCR','seychelles','SEYCHELLES','SEYCHELLES','SEYCHELLES','SEYCHELLERNE','SEYCHELLES'),(694,6255146,0,'SL','SLE','SL',32,'SLL','sierra leone','SIERRA LEONE','SIERRA LEONE','SIERRA LEONA','SIERRA LEONE','SIERRA LEONA'),(702,6255147,1,'SG','SGP','SN',36,'SGD','singapour','SINGAPOUR','SINGAPORE','SINGAPUR','SINGAPORE','SINGAPUR'),(703,6255148,1,'SK','SVK','LO',39,'SKK','slovaquie','SLOVAQUIE','SLOVAKIA','ESLOVAQUIA','SLOVAKIET','ESLOVAQUIA'),(704,6255147,1,'VN','VNM','VM',36,'VND','viet nam','VIET NAM','VIET NAM','VIETNAM','VIETNAM','VIETNAM'),(705,6255148,1,'SI','SVN','SI',39,'SIT','slovénie','SLOVÉNIE','SLOVENIA','ESLOVENIA','SLOVENIEN','ESLOVENIA'),(706,6255146,0,'SO','SOM','SO',32,'SOS','somalie','SOMALIE','SOMALIA','SOMALIA','SOMALIA','SOMALIA'),(710,6255146,1,'ZA','ZAF','SF',36,'ZAR','AFRIQUE DU SUD','AFRIQUE DU SUD','SOUTH AFRICA','SUDÁFRICA','SYDAFRIKA','SUDÁFRICA'),(716,6255146,0,'ZW','ZWE','ZI',32,'ZWD','zimbabwe','ZIMBABWE','ZIMBABWE','ZIMBABUE','ZIMBABWE','ZIMBABUE'),(724,6255148,1,'ES','ESP','SP',39,'EUR','espagne','ESPAGNE','SPAIN','ESPAÑA','SPANIEN','ESPAÑA'),(732,6255146,0,'EH','ESH','WI',32,'MAD','sahara occidental','SAHARA OCCIDENTAL','WESTERN SAHARA','SÁHARA OCCIDENTAL','VESTSAHARA','SÁHARA OCCIDENTAL'),(736,6255146,0,'SD','SDN','SU',32,'SDG','soudan','SOUDAN','SUDAN','SUDÁN','SUDAN','SUDÁN'),(740,6255150,0,'SR','SUR','NS',32,'SRG','suriname','SURINAME','SURINAME','SURINAM','SURINAM','SURINAM'),(744,6255148,0,'SJ','SJM','SV',32,'NOK','svalbard et île jan mayen','SVALBARD ET ÎLE JAN MAYEN','SVALBARD AND JAN MAYEN','SVALBARD Y JAN MAYEN','SVALBARD OG JAN MAYEN','SVALBARD Y JAN MAYEN'),(748,6255146,0,'SZ','SWZ','WZ',32,'SZL','swaziland','SWAZILAND','SWAZILAND','SUAZILANDIA','SWAZILAND','SUAZILANDIA'),(752,6255148,1,'SE','SWE','SW',39,'SEK','suède','SUÈDE','SWEDEN','SUECIA','SVERIGE','SUECIA'),(756,6255148,1,'CH','CHE','SZ',36,'CHF','suisse','SUISSE','SWITZERLAND','SUIZA','SCHWEIZ','SUIZA'),(760,6255147,0,'SY','SYR','SY',32,'SYP','syrienne, république arabe','SYRIENNE, RÉPUBLIQUE ARABE','SYRIAN ARAB REPUBLIC','SIRIA','SYRIEN','SIRIA'),(762,6255147,0,'TJ','TJK','TI',32,'TJR','tadjikistan','TADJIKISTAN','TAJIKISTAN','TAYIKISTÁN','TADSJIKISTAN','TAYIKISTÁN'),(764,6255147,1,'TH','THA','TH',36,'THB','thaïlande','THAÏLANDE','THAILAND','TAILANDIA','THAILAND','TAILANDIA'),(768,6255146,1,'TG','TGO','TO',36,'XAF','togo','TOGO','TOGO','TOGO','TOGO','TOGO'),(772,6255151,0,'TK','TKL','TL',32,'NZD','tokelau','TOKELAU','TOKELAU','TOKELAU','TOKELAU','TOKELAU'),(776,6255151,0,'TO','TON','TN',32,'TOP','tonga','TONGA','TONGA','TONGA','TONGA','TONGA'),(780,6255149,0,'TT','TTO','TD',32,'TTD','trinité-et-tobago','TRINITÉ-ET-TOBAGO','TRINIDAD AND TOBAGO','TRINIDAD Y TOBAGO','TRINIDAD OG TOBAGO','TRINIDAD Y TOBAGO'),(784,6255147,1,'AE','ARE','AE',36,'AED','émirats arabes unis','ÉMIRATS ARABES UNIS','UNITED ARAB EMIRATES','EMIRATOS ÁRABES UNIDOS','DE FORENEDE ARABISKE EMIRATER','EMIRATOS ÁRABES UNIDOS'),(788,6255146,1,'TN','TUN','TS',36,'TND','tunisie','TUNISIE','TUNISIA','TÚNEZ','TUNESIEN','TÚNEZ'),(792,6255147,1,'TR','TUR','TU',36,'TRL','turquie','TURQUIE','TURKEY','TURQUÍA','TYRKIET','TURQUÍA'),(795,6255147,0,'TM','TKM','TX',32,'TMM','turkménistan','TURKMÉNISTAN','TURKMENISTAN','TURKMENISTÁN','TURKMENISTAN','TURKMENISTÁN'),(796,6255149,0,'TC','TCA','TK',32,'USD','turks et caïques, îles','TURKS ET CAÏQUES, ÎLES','TURKS AND CAICOS ISLANDS','ISLAS TURCAS Y CAICOS','TURKS- OG CAICOSØERNE','ISLAS TURCAS Y CAICOS'),(798,6255151,0,'TV','TUV','TV',32,'AUD','tuvalu','TUVALU','TUVALU','TUVALU','TUVALU','TUVALU'),(800,6255146,0,'UG','UGA','UG',32,'UGS','ouganda','OUGANDA','UGANDA','UGANDA','UGANDA','UGANDA'),(804,6255148,1,'UA','UKR','UP',36,'UAH','ukraine','UKRAINE','UKRAINE','UCRANIA','UKRAINE','UCRANIA'),(807,6255148,1,'MK','MKD','MK',36,'MKD','macédoine','MACÉDOINE','MACEDONIA','MACEDONIA','DEN TIDLIGERE JUGOSLAVISKE REPUBLIK MAKEDONIEN','MACEDONIA'),(818,6255146,1,'EG','EGY','EG',36,'EGP','égypte','ÉGYPTE','EGYPT','EGIPTO','EGYPTEN','EGIPTO'),(826,6255148,1,'GB','GBR','UK',39,'GBP','royaume-uni','ROYAUME-UNI','UNITED KINGDOM','REINO UNIDO','DET FORENEDE KONGERIGE','REINO UNIDO'),(831,6255148,1,'GG','GGY','GK',36,'EUR','guernesey','GUERNESEY','GUERNSEY','GUERNSEY','GUERNSEY','GUERNSEY'),(832,6255148,1,'JE','JEY','JE',36,'EUR','jersey','JERSEY','JERSEY','JERSEY','JERSEY','JERSEY'),(833,6255148,0,'IM','IMN','IM',32,'EUR','île de man','ÎLE DE MAN','ISLE OF MAN','ISLE OF MAN','ISLE OF MAN','ISLE OF MAN'),(834,6255146,0,'TZ','TZA','TZ',32,'TZS','tanzanie, république-unie de','TANZANIE, RÉPUBLIQUE-UNIE DE','TANZANIA, UNITED REPUBLIC OF','TANZANIA','TANZANIA','TANZANIA'),(840,6255149,1,'US','USA','US',36,'USD','états-unis','ÉTATS-UNIS','UNITED STATES','ESTADOS UNIDOS','USA','ESTADOS UNIDOS'),(850,6255149,0,'VI','VIR','VQ',32,'USD','îles vierges des états-unis','ÎLES VIERGES DES ÉTATS-UNIS','VIRGIN ISLANDS, U.S.','ISLAS VÍRGENES AMERICANAS','DE AMERIKANSKE JOMFRUØER','ISLAS VÍRGENES AMERICANAS'),(854,6255146,0,'BF','BFA','UV',32,'XAF','burkina faso','BURKINA FASO','BURKINA FASO','BURKINA FASO','BURKINA FASO','BURKINA FASO'),(858,6255150,1,'UY','URY','UY',36,'UYU','uruguay','URUGUAY','URUGUAY','URUGUAY','URUGUAY','URUGUAY'),(860,6255147,1,'UZ','UZB','UZ',36,'UZS','ouzbékistan','OUZBÉKISTAN','UZBEKISTAN','UZBEKISTÁN','USBEKISTAN','UZBEKISTÁN'),(862,6255150,1,'VE','VEN','VE',36,'VEB','venezuela','VENEZUELA','VENEZUELA','VENEZUELA','VENEZUELA','VENEZUELA'),(876,6255151,1,'WF','WLF','WF',55,'XPF','WALLIS ET FUTUNA','WALLIS ET FUTUNA','WALLIS AND FUTUNA','WALLIS Y FUTUNA','WALLIS OG FUTUNAØERNE','WALLIS Y FUTUNA'),(882,6255151,0,'WS','WSM','WS',32,'WST','samoa','SAMOA','SAMOA','SAMOA','SAMOA','SAMOA'),(887,6255147,1,'YE','YEM','YM',36,'YER','yémen','YÉMEN','YEMEN','YEMEN','YEMEN','YEMEN'),(894,6255146,0,'ZM','ZMB','ZA',32,'ZMK','zambie','ZAMBIE','ZAMBIA','ZAMBIA','ZAMBIA','ZAMBIA');
/*!40000 ALTER TABLE `core_country` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_country_ip`
--

DROP TABLE IF EXISTS `core_country_ip`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_country_ip` (
  `from_ip` int(10) unsigned NOT NULL,
  `to_ip` int(10) unsigned NOT NULL,
  `country_id` smallint(5) unsigned NOT NULL,
  PRIMARY KEY (`from_ip`,`to_ip`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_country_ip`
--

LOCK TABLES `core_country_ip` WRITE;
/*!40000 ALTER TABLE `core_country_ip` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_country_ip` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_credit`
--

DROP TABLE IF EXISTS `core_credit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_credit` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `amount` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `credited_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `expire_on` date NOT NULL DEFAULT '0000-00-00',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `movement_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `order_id` int(10) unsigned NOT NULL DEFAULT '0',
  `sponsor_id` int(10) unsigned NOT NULL DEFAULT '0',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`),
  KEY `sponsor_id` (`sponsor_id`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_credit`
--

LOCK TABLES `core_credit` WRITE;
/*!40000 ALTER TABLE `core_credit` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_credit` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_criterion`
--

DROP TABLE IF EXISTS `core_criterion`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_criterion` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `position` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `model_id` (`model_id`)
) ENGINE=MyISAM AUTO_INCREMENT=10 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_criterion`
--

LOCK TABLES `core_criterion` WRITE;
/*!40000 ALTER TABLE `core_criterion` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_criterion` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_criterion_valuation`
--

DROP TABLE IF EXISTS `core_criterion_valuation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_criterion_valuation` (
  `criterion_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `valuation_id` int(10) unsigned NOT NULL DEFAULT '0',
  `grade` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`valuation_id`,`criterion_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_criterion_valuation`
--

LOCK TABLES `core_criterion_valuation` WRITE;
/*!40000 ALTER TABLE `core_criterion_valuation` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_criterion_valuation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_currency`
--

DROP TABLE IF EXISTS `core_currency`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_currency` (
  `id` smallint(5) unsigned NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `code` varchar(3) NOT NULL,
  `rate` float(9,4) NOT NULL DEFAULT '0.0000',
  `name_fr` varchar(255) NOT NULL,
  `name_en` varchar(64) NOT NULL,
  `symbol_left` varchar(16) NOT NULL,
  `symbol_right` varchar(16) NOT NULL,
  `decimal_point` varchar(1) NOT NULL DEFAULT '.',
  `thousands_point` varchar(1) NOT NULL,
  `decimal_places` varchar(1) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `code` (`code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_currency`
--

LOCK TABLES `core_currency` WRITE;
/*!40000 ALTER TABLE `core_currency` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_currency` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_debit`
--

DROP TABLE IF EXISTS `core_debit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_debit` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `amount` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `debited_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `name` varchar(255) NOT NULL,
  `order_id` int(10) unsigned NOT NULL DEFAULT '0',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`),
  KEY `item` (`model_id`,`record_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_debit`
--

LOCK TABLES `core_debit` WRITE;
/*!40000 ALTER TABLE `core_debit` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_debit` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_delivery`
--

DROP TABLE IF EXISTS `core_delivery`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_delivery` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `clicked_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `clicks` text NOT NULL COMMENT 'adresse de destination:heure',
  `letter_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `email_id` int(10) unsigned NOT NULL DEFAULT '0',
  `failed_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `delivered_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `name` varchar(255) NOT NULL,
  `opened_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `parts` text NOT NULL,
  `unsubscribed_on` date NOT NULL DEFAULT '0000-00-00',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `email_id` (`email_id`),
  KEY `letter_id` (`letter_id`)
) ENGINE=MyISAM AUTO_INCREMENT=9993 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_delivery`
--

LOCK TABLES `core_delivery` WRITE;
/*!40000 ALTER TABLE `core_delivery` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_delivery` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_developer`
--

DROP TABLE IF EXISTS `core_developer`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_developer` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `content` text NOT NULL,
  `description` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `user_id` (`user_id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_developer`
--

LOCK TABLES `core_developer` WRITE;
/*!40000 ALTER TABLE `core_developer` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_developer` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_discount`
--

DROP TABLE IF EXISTS `core_discount`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_discount` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `app` varchar(32) NOT NULL,
  `available_from` date NOT NULL DEFAULT '0000-00-00',
  `available_to` date NOT NULL DEFAULT '0000-00-00',
  `code` varchar(64) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `position` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `priority` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `quota` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`),
  KEY `status` (`status`),
  KEY `item` (`model_id`,`record_id`)
) ENGINE=MyISAM AUTO_INCREMENT=61 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_discount`
--

LOCK TABLES `core_discount` WRITE;
/*!40000 ALTER TABLE `core_discount` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_discount` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_discount_action`
--

DROP TABLE IF EXISTS `core_discount_action`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_discount_action` (
  `id` mediumint(8) unsigned NOT NULL AUTO_INCREMENT,
  `attribute` varchar(64) NOT NULL,
  `discount_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `rate` varchar(8) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `target` varchar(64) NOT NULL DEFAULT 'amount',
  `threshold` float(6,2) unsigned NOT NULL DEFAULT '0.00',
  `threshold_type` varchar(8) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL COMMENT 'amount,quantity',
  `value` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `discount_id` (`discount_id`)
) ENGINE=MyISAM AUTO_INCREMENT=59 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_discount_action`
--

LOCK TABLES `core_discount_action` WRITE;
/*!40000 ALTER TABLE `core_discount_action` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_discount_action` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_discount_condition`
--

DROP TABLE IF EXISTS `core_discount_condition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_discount_condition` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `attribute` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `discount_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `threshold` float(6,2) unsigned NOT NULL DEFAULT '0.00',
  `threshold_type` varchar(8) NOT NULL COMMENT 'amount|quantity',
  `value` varchar(510) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `discount_id` (`discount_id`),
  KEY `attribute` (`attribute`)
) ENGINE=MyISAM AUTO_INCREMENT=93 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_discount_condition`
--

LOCK TABLES `core_discount_condition` WRITE;
/*!40000 ALTER TABLE `core_discount_condition` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_discount_condition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_discount_order`
--

DROP TABLE IF EXISTS `core_discount_order`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_discount_order` (
  `discount_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `order_id` int(10) unsigned NOT NULL DEFAULT '0',
  `amount` float(6,2) unsigned NOT NULL DEFAULT '0.00',
  KEY `order_id` (`order_id`),
  KEY `discount_id` (`discount_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_discount_order`
--

LOCK TABLES `core_discount_order` WRITE;
/*!40000 ALTER TABLE `core_discount_order` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_discount_order` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_discount_purchase`
--

DROP TABLE IF EXISTS `core_discount_purchase`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_discount_purchase` (
  `discount_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `purchase_id` int(10) unsigned NOT NULL DEFAULT '0',
  `amount` float(6,2) unsigned NOT NULL DEFAULT '0.00',
  KEY `purchase_id` (`purchase_id`),
  KEY `discount_id` (`discount_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_discount_purchase`
--

LOCK TABLES `core_discount_purchase` WRITE;
/*!40000 ALTER TABLE `core_discount_purchase` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_discount_purchase` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_discussion`
--

DROP TABLE IF EXISTS `core_discussion`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_discussion` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `content` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `group_id` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `group_id` (`group_id`),
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_discussion`
--

LOCK TABLES `core_discussion` WRITE;
/*!40000 ALTER TABLE `core_discussion` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_discussion` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_document`
--

DROP TABLE IF EXISTS `core_document`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_document` (
  `id` mediumint(8) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `size` smallint(5) unsigned NOT NULL DEFAULT '0',
  `ranking` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `item` (`model_id`,`record_id`,`locale`),
  KEY `status` (`status`)
) ENGINE=MyISAM AUTO_INCREMENT=132563 DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_document`
--

LOCK TABLES `core_document` WRITE;
/*!40000 ALTER TABLE `core_document` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_document` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_draft`
--

DROP TABLE IF EXISTS `core_draft`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_draft` (
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `fields` text NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`model_id`,`record_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_draft`
--

LOCK TABLES `core_draft` WRITE;
/*!40000 ALTER TABLE `core_draft` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_draft` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_email`
--

DROP TABLE IF EXISTS `core_email`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_email` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `email` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `datas` text NOT NULL,
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(8) NOT NULL DEFAULT '4',
  PRIMARY KEY (`id`),
  UNIQUE KEY `email` (`email`)
) ENGINE=MyISAM AUTO_INCREMENT=2349 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_email`
--

LOCK TABLES `core_email` WRITE;
/*!40000 ALTER TABLE `core_email` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_email` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_email_notification`
--

DROP TABLE IF EXISTS `core_email_notification`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_email_notification` (
  `email` varchar(255) NOT NULL,
  `notification_id` int(10) unsigned NOT NULL DEFAULT '0',
  KEY `notification_id` (`notification_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_email_notification`
--

LOCK TABLES `core_email_notification` WRITE;
/*!40000 ALTER TABLE `core_email_notification` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_email_notification` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_engine`
--

DROP TABLE IF EXISTS `core_engine`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_engine` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(255) NOT NULL,
  `content` text NOT NULL,
  `description` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `status` tinyint(1) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_engine`
--

LOCK TABLES `core_engine` WRITE;
/*!40000 ALTER TABLE `core_engine` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_engine` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_event`
--

DROP TABLE IF EXISTS `core_event`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_event` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `calendar_id` int(10) unsigned NOT NULL DEFAULT '0',
  `content` text NOT NULL,
  `description` text NOT NULL,
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `thumbnail` varchar(255) NOT NULL,
  `title` varchar(255) NOT NULL,
  `type` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  KEY `calendar_id` (`calendar_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_event`
--

LOCK TABLES `core_event` WRITE;
/*!40000 ALTER TABLE `core_event` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_event` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_execution`
--

DROP TABLE IF EXISTS `core_execution`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_execution` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `workflow_id` int(10) unsigned NOT NULL DEFAULT '0',
  `code` varchar(255) NOT NULL,
  `counter` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `inputs` text NOT NULL,
  `name` varchar(255) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `tasks` text NOT NULL,
  PRIMARY KEY (`id`),
  KEY `workflow_id` (`workflow_id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_execution`
--

LOCK TABLES `core_execution` WRITE;
/*!40000 ALTER TABLE `core_execution` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_execution` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_faq`
--

DROP TABLE IF EXISTS `core_faq`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_faq` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `about` text NOT NULL,
  `app` varchar(16) NOT NULL,
  `code` varchar(32) NOT NULL,
  `email` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `position` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_faq`
--

LOCK TABLES `core_faq` WRITE;
/*!40000 ALTER TABLE `core_faq` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_faq` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_favorite`
--

DROP TABLE IF EXISTS `core_favorite`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_favorite` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `comment` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`),
  KEY `item` (`model_id`,`record_id`)
) ENGINE=MyISAM AUTO_INCREMENT=148 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_favorite`
--

LOCK TABLES `core_favorite` WRITE;
/*!40000 ALTER TABLE `core_favorite` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_favorite` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_feed`
--

DROP TABLE IF EXISTS `core_feed`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_feed` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `accessed_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `description` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `url` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `item` (`model_id`,`record_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_feed`
--

LOCK TABLES `core_feed` WRITE;
/*!40000 ALTER TABLE `core_feed` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_feed` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_feedback`
--

DROP TABLE IF EXISTS `core_feedback`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_feedback` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `content` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_feedback`
--

LOCK TABLES `core_feedback` WRITE;
/*!40000 ALTER TABLE `core_feedback` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_feedback` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_field`
--

DROP TABLE IF EXISTS `core_field`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_field` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `class` varchar(128) NOT NULL,
  `code` varchar(64) NOT NULL,
  `control_type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `event` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `form_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `position` smallint(5) unsigned NOT NULL DEFAULT '0',
  `size` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  KEY `form_id` (`form_id`)
) ENGINE=MyISAM AUTO_INCREMENT=358 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_field`
--

LOCK TABLES `core_field` WRITE;
/*!40000 ALTER TABLE `core_field` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_field` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_field_submission`
--

DROP TABLE IF EXISTS `core_field_submission`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_field_submission` (
  `field_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `submission_id` int(10) unsigned NOT NULL DEFAULT '0',
  `value` text NOT NULL,
  PRIMARY KEY (`submission_id`,`field_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_field_submission`
--

LOCK TABLES `core_field_submission` WRITE;
/*!40000 ALTER TABLE `core_field_submission` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_field_submission` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_form`
--

DROP TABLE IF EXISTS `core_form`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_form` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(16) NOT NULL,
  `base_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `class` varchar(255) NOT NULL,
  `code` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `onsubmit` varchar(255) NOT NULL,
  `recipients` varchar(510) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `template` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_form`
--

LOCK TABLES `core_form` WRITE;
/*!40000 ALTER TABLE `core_form` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_form` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_forum`
--

DROP TABLE IF EXISTS `core_forum`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_forum` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(16) NOT NULL,
  `code` varchar(64) NOT NULL,
  `column_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `description` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `last_topic_id` int(10) unsigned NOT NULL DEFAULT '0',
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `position` int(10) unsigned NOT NULL DEFAULT '1',
  `status` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  KEY `position` (`position`),
  KEY `code` (`code`),
  KEY `column_id` (`column_id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_forum`
--

LOCK TABLES `core_forum` WRITE;
/*!40000 ALTER TABLE `core_forum` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_forum` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_friend`
--

DROP TABLE IF EXISTS `core_friend`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_friend` (
  `user_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `friend_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `friend_id` (`friend_id`),
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_friend`
--

LOCK TABLES `core_friend` WRITE;
/*!40000 ALTER TABLE `core_friend` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_friend` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_geolocation`
--

DROP TABLE IF EXISTS `core_geolocation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_geolocation` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `admin1` varchar(255) NOT NULL,
  `admin2` varchar(255) NOT NULL,
  `admin3` varchar(255) NOT NULL,
  `address` varchar(255) NOT NULL,
  `country` varchar(255) NOT NULL,
  `country_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `establishment` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `latitude` double NOT NULL DEFAULT '0',
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `locality` varchar(255) NOT NULL,
  `longitude` double NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `postal_code` varchar(128) NOT NULL,
  `pov_heading` double NOT NULL DEFAULT '0',
  `pov_latitude` double NOT NULL DEFAULT '0',
  `pov_longitude` double NOT NULL DEFAULT '0',
  `pov_pitch` double NOT NULL DEFAULT '0',
  `pov_zoom` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `radius` double unsigned NOT NULL DEFAULT '0',
  `route` varchar(255) NOT NULL,
  `street_number` varchar(64) NOT NULL,
  `sublocality` varchar(255) NOT NULL,
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `zoom` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `item` (`model_id`,`record_id`,`locale`)
) ENGINE=MyISAM AUTO_INCREMENT=399 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_geolocation`
--

LOCK TABLES `core_geolocation` WRITE;
/*!40000 ALTER TABLE `core_geolocation` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_geolocation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_giftcard`
--

DROP TABLE IF EXISTS `core_giftcard`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_giftcard` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `amount` float(9,2) unsigned NOT NULL,
  `content` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `stock` mediumint(8) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_giftcard`
--

LOCK TABLES `core_giftcard` WRITE;
/*!40000 ALTER TABLE `core_giftcard` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_giftcard` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_glossary`
--

DROP TABLE IF EXISTS `core_glossary`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_glossary` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(16) NOT NULL,
  `name` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_glossary`
--

LOCK TABLES `core_glossary` WRITE;
/*!40000 ALTER TABLE `core_glossary` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_glossary` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_goal`
--

DROP TABLE IF EXISTS `core_goal`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_goal` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `code` varchar(32) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `value` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_goal`
--

LOCK TABLES `core_goal` WRITE;
/*!40000 ALTER TABLE `core_goal` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_goal` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_google_area`
--

DROP TABLE IF EXISTS `core_google_area`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_google_area` (
  `admin1_id` int(10) unsigned NOT NULL DEFAULT '0',
  `admin2_id` int(10) unsigned NOT NULL DEFAULT '0',
  `city_id` int(10) unsigned NOT NULL DEFAULT '0',
  `country_id` smallint(5) unsigned NOT NULL DEFAULT '250',
  `postal_code` varchar(32) NOT NULL,
  PRIMARY KEY (`admin1_id`,`admin2_id`,`city_id`,`country_id`,`postal_code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_google_area`
--

LOCK TABLES `core_google_area` WRITE;
/*!40000 ALTER TABLE `core_google_area` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_google_area` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_google_cache`
--

DROP TABLE IF EXISTS `core_google_cache`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_google_cache` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `url` varchar(500) NOT NULL,
  `response` text NOT NULL,
  `count` int(10) unsigned NOT NULL DEFAULT '0',
  `requested_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `url` (`url`(333))
) ENGINE=MyISAM AUTO_INCREMENT=82 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_google_cache`
--

LOCK TABLES `core_google_cache` WRITE;
/*!40000 ALTER TABLE `core_google_cache` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_google_cache` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_google_name`
--

DROP TABLE IF EXISTS `core_google_name`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_google_name` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `country_id` smallint(5) unsigned NOT NULL DEFAULT '250',
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`,`locale`)
) ENGINE=MyISAM AUTO_INCREMENT=10 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_google_name`
--

LOCK TABLES `core_google_name` WRITE;
/*!40000 ALTER TABLE `core_google_name` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_google_name` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_group`
--

DROP TABLE IF EXISTS `core_group`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_group` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `content` text NOT NULL,
  `description` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL,
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM AUTO_INCREMENT=50 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_group`
--

LOCK TABLES `core_group` WRITE;
/*!40000 ALTER TABLE `core_group` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_group` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_group_user`
--

DROP TABLE IF EXISTS `core_group_user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_group_user` (
  `group_id` int(10) unsigned NOT NULL DEFAULT '0',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `group_id` (`group_id`),
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_group_user`
--

LOCK TABLES `core_group_user` WRITE;
/*!40000 ALTER TABLE `core_group_user` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_group_user` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_heading`
--

DROP TABLE IF EXISTS `core_heading`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_heading` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `blog_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `blog_id` (`blog_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_heading`
--

LOCK TABLES `core_heading` WRITE;
/*!40000 ALTER TABLE `core_heading` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_heading` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_heading_post`
--

DROP TABLE IF EXISTS `core_heading_post`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_heading_post` (
  `heading_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `post_id` int(10) unsigned NOT NULL DEFAULT '0',
  `position` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`heading_id`,`post_id`),
  KEY `post_id` (`post_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_heading_post`
--

LOCK TABLES `core_heading_post` WRITE;
/*!40000 ALTER TABLE `core_heading_post` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_heading_post` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_index`
--

DROP TABLE IF EXISTS `core_index`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_index` (
  `document_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `content` text NOT NULL,
  `string1` varchar(255) NOT NULL,
  `string2` varchar(255) NOT NULL,
  `string3` varchar(255) NOT NULL,
  `numeric1` double NOT NULL DEFAULT '0',
  `numeric2` double NOT NULL DEFAULT '0',
  `numeric3` double NOT NULL DEFAULT '0',
  `date1` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `date2` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `date3` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`document_id`),
  KEY `numeric1` (`numeric1`),
  KEY `string1` (`string1`),
  KEY `date1` (`date1`),
  FULLTEXT KEY `content` (`content`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_index`
--

LOCK TABLES `core_index` WRITE;
/*!40000 ALTER TABLE `core_index` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_index` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_installment`
--

DROP TABLE IF EXISTS `core_installment`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_installment` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `amount` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `code` varchar(255) NOT NULL,
  `datas` text NOT NULL,
  `description` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `payment_id` int(10) unsigned NOT NULL DEFAULT '0',
  `order_id` int(10) unsigned NOT NULL DEFAULT '0',
  `received_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `status` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `payment_id` (`payment_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_installment`
--

LOCK TABLES `core_installment` WRITE;
/*!40000 ALTER TABLE `core_installment` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_installment` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_instance`
--

DROP TABLE IF EXISTS `core_instance`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_instance` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `content` text NOT NULL,
  `description` text NOT NULL,
  `event_id` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `thumbnail` varchar(255) NOT NULL,
  `title` varchar(255) NOT NULL,
  `type` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  KEY `event_id` (`event_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_instance`
--

LOCK TABLES `core_instance` WRITE;
/*!40000 ALTER TABLE `core_instance` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_instance` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_interest`
--

DROP TABLE IF EXISTS `core_interest`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_interest` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `code` varchar(32) NOT NULL,
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=4 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_interest`
--

LOCK TABLES `core_interest` WRITE;
/*!40000 ALTER TABLE `core_interest` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_interest` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_invitation`
--

DROP TABLE IF EXISTS `core_invitation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_invitation` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `content` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `recipient_model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `recipient_record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `recipient_id` (`recipient_model_id`),
  KEY `user_id` (`user_id`),
  KEY `item` (`model_id`,`record_id`),
  KEY `recipient` (`recipient_model_id`,`recipient_record_id`)
) ENGINE=MyISAM AUTO_INCREMENT=224 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_invitation`
--

LOCK TABLES `core_invitation` WRITE;
/*!40000 ALTER TABLE `core_invitation` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_invitation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_item_bundle`
--

DROP TABLE IF EXISTS `core_item_bundle`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_item_bundle` (
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `bundle_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`model_id`,`record_id`,`bundle_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_item_bundle`
--

LOCK TABLES `core_item_bundle` WRITE;
/*!40000 ALTER TABLE `core_item_bundle` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_item_bundle` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_item_data`
--

DROP TABLE IF EXISTS `core_item_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_item_data` (
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `value` text NOT NULL,
  `updated_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  UNIQUE KEY `item` (`model_id`,`record_id`,`type`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_item_data`
--

LOCK TABLES `core_item_data` WRITE;
/*!40000 ALTER TABLE `core_item_data` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_item_data` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_item_discount`
--

DROP TABLE IF EXISTS `core_item_discount`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_item_discount` (
  `model_id` smallint(5) unsigned NOT NULL,
  `record_id` int(10) unsigned NOT NULL,
  `amount` float(6,2) unsigned NOT NULL DEFAULT '0.00',
  `discount_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `inserted_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `item` (`model_id`,`record_id`),
  KEY `discount_id` (`discount_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_item_discount`
--

LOCK TABLES `core_item_discount` WRITE;
/*!40000 ALTER TABLE `core_item_discount` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_item_discount` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_item_interaction`
--

DROP TABLE IF EXISTS `core_item_interaction`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_item_interaction` (
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `medium` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `origin` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `source` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `status` int(11) NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  `value` int(10) unsigned NOT NULL DEFAULT '0',
  `visit_id` int(10) unsigned NOT NULL DEFAULT '0',
  `created_on` date NOT NULL DEFAULT '0000-00-00',
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `item` (`model_id`,`record_id`),
  KEY `created_on` (`created_on`),
  KEY `type` (`type`),
  KEY `medium` (`medium`),
  KEY `visit_id` (`visit_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_item_interaction`
--

LOCK TABLES `core_item_interaction` WRITE;
/*!40000 ALTER TABLE `core_item_interaction` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_item_interaction` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_item_log`
--

DROP TABLE IF EXISTS `core_item_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_item_log` (
  `model_id` smallint(6) NOT NULL DEFAULT '0',
  `record_id` int(11) NOT NULL DEFAULT '0',
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `counter` mediumint(9) NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `created_on` date NOT NULL DEFAULT '0000-00-00',
  PRIMARY KEY (`model_id`,`record_id`,`created_on`),
  KEY `created_on` (`created_on`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_item_log`
--

LOCK TABLES `core_item_log` WRITE;
/*!40000 ALTER TABLE `core_item_log` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_item_log` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_item_meta`
--

DROP TABLE IF EXISTS `core_item_meta`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_item_meta` (
  `model` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record` int(10) unsigned NOT NULL DEFAULT '0',
  `abuses` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `admin` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `bits` int(10) unsigned NOT NULL DEFAULT '0',
  `child` int(10) unsigned NOT NULL DEFAULT '0',
  `cluster` smallint(5) unsigned NOT NULL DEFAULT '0',
  `comments` smallint(5) unsigned NOT NULL DEFAULT '0',
  `conversions` smallint(5) unsigned NOT NULL DEFAULT '0',
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `dislikes` smallint(5) unsigned NOT NULL DEFAULT '0',
  `displays` int(10) unsigned NOT NULL DEFAULT '0',
  `downloads` smallint(5) unsigned NOT NULL DEFAULT '0',
  `favorites` smallint(5) unsigned NOT NULL DEFAULT '0',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `interactions` smallint(5) unsigned NOT NULL DEFAULT '0',
  `interests` int(10) unsigned NOT NULL DEFAULT '0',
  `latitude` double NOT NULL DEFAULT '0',
  `likes` smallint(5) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `longitude` double NOT NULL DEFAULT '0',
  `node` int(10) unsigned NOT NULL DEFAULT '0',
  `parent` int(10) unsigned NOT NULL DEFAULT '0',
  `rating` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `revision` smallint(5) unsigned NOT NULL DEFAULT '0',
  `shares` smallint(5) unsigned NOT NULL DEFAULT '0',
  `state` smallint(5) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `subscriptions` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `updated_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `user` int(10) unsigned NOT NULL DEFAULT '0',
  `valuations` smallint(5) unsigned NOT NULL DEFAULT '0',
  `visit` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`model`,`record`),
  KEY `created_at` (`created_at`),
  KEY `updated_at` (`updated_at`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_item_meta`
--

LOCK TABLES `core_item_meta` WRITE;
/*!40000 ALTER TABLE `core_item_meta` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_item_meta` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_item_node`
--

DROP TABLE IF EXISTS `core_item_node`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_item_node` (
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `node_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `postion` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`model_id`,`record_id`,`node_id`),
  KEY `node_id` (`node_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_item_node`
--

LOCK TABLES `core_item_node` WRITE;
/*!40000 ALTER TABLE `core_item_node` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_item_node` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_item_property`
--

DROP TABLE IF EXISTS `core_item_property`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_item_property` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `property_id` smallint(5) unsigned NOT NULL DEFAULT '1',
  `description` text NOT NULL,
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `value` text NOT NULL,
  `draft` text NOT NULL,
  `accepted_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `updated_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `item` (`model_id`,`record_id`,`property_id`,`locale`)
) ENGINE=MyISAM AUTO_INCREMENT=146307 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_item_property`
--

LOCK TABLES `core_item_property` WRITE;
/*!40000 ALTER TABLE `core_item_property` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_item_property` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_item_revision`
--

DROP TABLE IF EXISTS `core_item_revision`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_item_revision` (
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `creator_model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `creator_record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `action` varchar(255) NOT NULL,
  `attributes` text NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `item` (`model_id`,`record_id`),
  KEY `creator` (`creator_model_id`,`creator_record_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_item_revision`
--

LOCK TABLES `core_item_revision` WRITE;
/*!40000 ALTER TABLE `core_item_revision` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_item_revision` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_item_seo`
--

DROP TABLE IF EXISTS `core_item_seo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_item_seo` (
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `canonical_url` varchar(255) NOT NULL,
  `description` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `keywords` varchar(255) NOT NULL,
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `slug` varchar(255) NOT NULL,
  `title` varchar(255) NOT NULL,
  `tracking_category` varchar(64) NOT NULL,
  `tracking_label` varchar(64) NOT NULL,
  PRIMARY KEY (`model_id`,`record_id`,`locale`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_item_seo`
--

LOCK TABLES `core_item_seo` WRITE;
/*!40000 ALTER TABLE `core_item_seo` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_item_seo` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_item_tag`
--

DROP TABLE IF EXISTS `core_item_tag`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_item_tag` (
  `model_id` smallint(5) unsigned NOT NULL,
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `tag_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `created_on` date NOT NULL DEFAULT '0000-00-00',
  PRIMARY KEY (`model_id`,`record_id`,`tag_id`,`locale`),
  KEY `tag_id` (`tag_id`),
  KEY `created_on` (`created_on`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_item_tag`
--

LOCK TABLES `core_item_tag` WRITE;
/*!40000 ALTER TABLE `core_item_tag` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_item_tag` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_item_tracer`
--

DROP TABLE IF EXISTS `core_item_tracer`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_item_tracer` (
  `session` int(10) unsigned NOT NULL DEFAULT '0',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `code` varchar(255) NOT NULL,
  `inserted_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `visit_id` (`session`),
  KEY `model_id` (`model_id`,`record_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_item_tracer`
--

LOCK TABLES `core_item_tracer` WRITE;
/*!40000 ALTER TABLE `core_item_tracer` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_item_tracer` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_lander`
--

DROP TABLE IF EXISTS `core_lander`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_lander` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `code` varchar(32) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `code` (`code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_lander`
--

LOCK TABLES `core_lander` WRITE;
/*!40000 ALTER TABLE `core_lander` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_lander` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_language`
--

DROP TABLE IF EXISTS `core_language`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_language` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `bit` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `code` varchar(3) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `name_en` varchar(255) NOT NULL,
  `name_native` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '-1',
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=9 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_language`
--

LOCK TABLES `core_language` WRITE;
/*!40000 ALTER TABLE `core_language` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_language` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_layout`
--

DROP TABLE IF EXISTS `core_layout`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_layout` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `description` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `online_from` date NOT NULL DEFAULT '0000-00-00',
  `online_to` date NOT NULL DEFAULT '0000-00-00',
  `ratio` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '-50',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_layout`
--

LOCK TABLES `core_layout` WRITE;
/*!40000 ALTER TABLE `core_layout` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_layout` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_lead`
--

DROP TABLE IF EXISTS `core_lead`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_lead` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `assigned_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `assignments` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `call_date` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `call_threshold` tinyint(3) unsigned NOT NULL DEFAULT '3',
  `called_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `calls` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `code` varchar(128) NOT NULL,
  `contact_id` int(10) unsigned NOT NULL DEFAULT '0',
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `customer_num` varchar(64) NOT NULL,
  `datas` text NOT NULL,
  `description` text NOT NULL,
  `dispatched_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `expert_id` int(10) unsigned NOT NULL DEFAULT '0',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `form_id` int(10) unsigned NOT NULL DEFAULT '0',
  `lead_id` int(10) unsigned NOT NULL DEFAULT '0',
  `motive` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `priority` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `remote_id` int(10) unsigned NOT NULL DEFAULT '0',
  `sector` varchar(32) NOT NULL,
  `status` tinyint(3) NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=24 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_lead`
--

LOCK TABLES `core_lead` WRITE;
/*!40000 ALTER TABLE `core_lead` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_lead` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_lead_beneficiary`
--

DROP TABLE IF EXISTS `core_lead_beneficiary`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_lead_beneficiary` (
  `lead_id` int(10) unsigned NOT NULL DEFAULT '0',
  `beneficiary` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`lead_id`,`beneficiary`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_lead_beneficiary`
--

LOCK TABLES `core_lead_beneficiary` WRITE;
/*!40000 ALTER TABLE `core_lead_beneficiary` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_lead_beneficiary` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_letter`
--

DROP TABLE IF EXISTS `core_letter`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_letter` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `about` tinytext NOT NULL,
  `app` varchar(32) NOT NULL,
  `code` varchar(128) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `from_email` varchar(255) NOT NULL,
  `from_name` varchar(255) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `recipients` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '20',
  PRIMARY KEY (`id`),
  KEY `code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=126 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_letter`
--

LOCK TABLES `core_letter` WRITE;
/*!40000 ALTER TABLE `core_letter` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_letter` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_license`
--

DROP TABLE IF EXISTS `core_license`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_license` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `alerts` int(10) unsigned NOT NULL DEFAULT '0',
  `code` varchar(32) NOT NULL,
  `duration` smallint(5) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `price` float(6,2) unsigned NOT NULL DEFAULT '0.00',
  `privilege_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `tax` float(4,2) NOT NULL DEFAULT '19.60',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=16 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_license`
--

LOCK TABLES `core_license` WRITE;
/*!40000 ALTER TABLE `core_license` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_license` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_locale`
--

DROP TABLE IF EXISTS `core_locale`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_locale` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `bit` int(10) unsigned NOT NULL DEFAULT '0',
  `code` char(5) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `lang` varchar(16) NOT NULL,
  `language_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `native` varchar(64) NOT NULL,
  `region_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`),
  KEY `region_id` (`region_id`),
  KEY `language_id` (`language_id`)
) ENGINE=MyISAM AUTO_INCREMENT=27 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_locale`
--

LOCK TABLES `core_locale` WRITE;
/*!40000 ALTER TABLE `core_locale` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_locale` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_mailing`
--

DROP TABLE IF EXISTS `core_mailing`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_mailing` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `bases` varchar(255) NOT NULL,
  `content` text NOT NULL,
  `description` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `scheduled_at` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `subject` varchar(255) NOT NULL,
  `template` varchar(255) NOT NULL DEFAULT 'push:skeleton.mailing',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_mailing`
--

LOCK TABLES `core_mailing` WRITE;
/*!40000 ALTER TABLE `core_mailing` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_mailing` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_mailing_item`
--

DROP TABLE IF EXISTS `core_mailing_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_mailing_item` (
  `mailing_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `position` smallint(5) unsigned NOT NULL DEFAULT '0',
  KEY `mailing_id` (`mailing_id`),
  KEY `item` (`model_id`,`record_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_mailing_item`
--

LOCK TABLES `core_mailing_item` WRITE;
/*!40000 ALTER TABLE `core_mailing_item` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_mailing_item` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_merchant`
--

DROP TABLE IF EXISTS `core_merchant`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_merchant` (
  `id` tinyint(3) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(16) NOT NULL,
  `name` varchar(255) NOT NULL,
  `corporate_name` varchar(255) NOT NULL,
  `corporate_code` varchar(255) NOT NULL,
  `corporate_type` varchar(255) NOT NULL,
  `legal_form` varchar(255) NOT NULL,
  `tax_authority_reference` varchar(255) NOT NULL,
  `address` varchar(255) NOT NULL,
  `caption` varchar(255) NOT NULL,
  `address_extra` varchar(255) NOT NULL,
  `postal_code` varchar(255) NOT NULL,
  `city` varchar(255) NOT NULL,
  `country` varchar(255) NOT NULL,
  `phone` varchar(255) NOT NULL,
  `fax` varchar(255) NOT NULL,
  `email` varchar(255) NOT NULL,
  `image` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_merchant`
--

LOCK TABLES `core_merchant` WRITE;
/*!40000 ALTER TABLE `core_merchant` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_merchant` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_message`
--

DROP TABLE IF EXISTS `core_message`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_message` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `content` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `group_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `hashtable` text NOT NULL,
  `replier_id` int(10) unsigned NOT NULL DEFAULT '0',
  `title` varchar(255) NOT NULL,
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM AUTO_INCREMENT=401 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_message`
--

LOCK TABLES `core_message` WRITE;
/*!40000 ALTER TABLE `core_message` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_message` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_message_user`
--

DROP TABLE IF EXISTS `core_message_user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_message_user` (
  `message_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`message_id`,`user_id`),
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_message_user`
--

LOCK TABLES `core_message_user` WRITE;
/*!40000 ALTER TABLE `core_message_user` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_message_user` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_method`
--

DROP TABLE IF EXISTS `core_method`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_method` (
  `id` tinyint(3) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `letter_pending` varchar(32) NOT NULL,
  `letter_received` varchar(32) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `options` varchar(255) NOT NULL,
  `position` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `psp_id` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `snippet_response` varchar(32) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `psp_id` (`psp_id`)
) ENGINE=MyISAM AUTO_INCREMENT=31 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_method`
--

LOCK TABLES `core_method` WRITE;
/*!40000 ALTER TABLE `core_method` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_method` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_motive`
--

DROP TABLE IF EXISTS `core_motive`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_motive` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(16) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `position` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `recipients` varchar(255) NOT NULL,
  `threshold` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=utf8 COMMENT='motif d''abus';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_motive`
--

LOCK TABLES `core_motive` WRITE;
/*!40000 ALTER TABLE `core_motive` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_motive` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_movement`
--

DROP TABLE IF EXISTS `core_movement`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_movement` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `amount` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `code` varchar(64) NOT NULL,
  `description` text NOT NULL,
  `duration` smallint(5) unsigned NOT NULL DEFAULT '365',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `label` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_movement`
--

LOCK TABLES `core_movement` WRITE;
/*!40000 ALTER TABLE `core_movement` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_movement` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_newsletter`
--

DROP TABLE IF EXISTS `core_newsletter`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_newsletter` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `description` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_newsletter`
--

LOCK TABLES `core_newsletter` WRITE;
/*!40000 ALTER TABLE `core_newsletter` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_newsletter` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_node`
--

DROP TABLE IF EXISTS `core_node`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_node` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `ancestor_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `code` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `path` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `position` smallint(5) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `tree_id` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `value` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `path` (`path`),
  KEY `item` (`model_id`,`record_id`),
  KEY `position` (`position`),
  KEY `ancestor_id` (`ancestor_id`)
) ENGINE=MyISAM AUTO_INCREMENT=4990 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_node`
--

LOCK TABLES `core_node` WRITE;
/*!40000 ALTER TABLE `core_node` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_node` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_notice`
--

DROP TABLE IF EXISTS `core_notice`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_notice` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `code` varchar(16) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `position` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `signup_display` tinyint(1) unsigned NOT NULL DEFAULT '0' COMMENT 'apparait lors de l''inscription',
  `status` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  KEY `code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_notice`
--

LOCK TABLES `core_notice` WRITE;
/*!40000 ALTER TABLE `core_notice` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_notice` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_notification`
--

DROP TABLE IF EXISTS `core_notification`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_notification` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `comment` text NOT NULL,
  `service_id` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `item` (`model_id`,`record_id`)
) ENGINE=MyISAM AUTO_INCREMENT=64 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_notification`
--

LOCK TABLES `core_notification` WRITE;
/*!40000 ALTER TABLE `core_notification` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_notification` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_oauth_login`
--

DROP TABLE IF EXISTS `core_oauth_login`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_oauth_login` (
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `provider` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `email` varchar(255) NOT NULL,
  `uid` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `code` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `access_token` varchar(500) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `refresh_token` varchar(500) NOT NULL,
  `profile` text NOT NULL,
  `oauth` text CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `inserted_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  KEY `user_id` (`user_id`),
  KEY `uid` (`uid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_oauth_login`
--

LOCK TABLES `core_oauth_login` WRITE;
/*!40000 ALTER TABLE `core_oauth_login` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_oauth_login` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_oauth_token`
--

DROP TABLE IF EXISTS `core_oauth_token`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_oauth_token` (
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `provider` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `scope` text NOT NULL,
  `code` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `access_token` varchar(500) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `refresh_token` varchar(500) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `oauth` text CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `refreshed_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_oauth_token`
--

LOCK TABLES `core_oauth_token` WRITE;
/*!40000 ALTER TABLE `core_oauth_token` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_oauth_token` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_offer`
--

DROP TABLE IF EXISTS `core_offer`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_offer` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `content` text NOT NULL,
  `delivered_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `from_email` varchar(255) NOT NULL,
  `from_name` varchar(255) NOT NULL,
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `notice_id` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'desinscription',
  `recipients` varchar(255) NOT NULL,
  `reply_email` varchar(255) NOT NULL,
  `scheduled_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `subject` varchar(255) NOT NULL,
  `user_flags` int(10) unsigned NOT NULL DEFAULT '0',
  `user_notices` int(10) unsigned NOT NULL DEFAULT '0',
  `user_privileges` int(10) unsigned NOT NULL DEFAULT '0',
  `user_profiles` int(10) unsigned NOT NULL DEFAULT '0',
  `user_statuses` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_offer`
--

LOCK TABLES `core_offer` WRITE;
/*!40000 ALTER TABLE `core_offer` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_offer` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_order`
--

DROP TABLE IF EXISTS `core_order`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_order` (
  `id` mediumint(8) unsigned NOT NULL AUTO_INCREMENT,
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `addressee_id` int(10) unsigned NOT NULL DEFAULT '0',
  `carrier_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `checkedout_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `code` varchar(64) NOT NULL,
  `comment` varchar(255) NOT NULL,
  `completed_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `coupon` varchar(16) NOT NULL,
  `discount_amount` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `error_amount` float(9,2) NOT NULL DEFAULT '0.00',
  `expire_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `fianet_response` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `handling_amount` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `invoice_id` int(10) unsigned NOT NULL DEFAULT '0',
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `merchant_id` smallint(5) unsigned NOT NULL DEFAULT '1',
  `method_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `paid_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `purchase_amount` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `shipping_amount` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `shipping_admin2` varchar(16) NOT NULL,
  `shipping_country_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `shipping_method` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `started_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `status` tinyint(3) NOT NULL DEFAULT '0',
  `tax_amount` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `total_amount` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  `user_ip` varchar(255) NOT NULL,
  `warehouse_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`),
  KEY `paid_at` (`paid_at`),
  KEY `checkedout_at` (`checkedout_at`),
  KEY `status` (`status`)
) ENGINE=MyISAM AUTO_INCREMENT=4695 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_order`
--

LOCK TABLES `core_order` WRITE;
/*!40000 ALTER TABLE `core_order` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_order` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_origin`
--

DROP TABLE IF EXISTS `core_origin`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_origin` (
  `id` tinyint(3) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(16) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `position` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_origin`
--

LOCK TABLES `core_origin` WRITE;
/*!40000 ALTER TABLE `core_origin` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_origin` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_pack`
--

DROP TABLE IF EXISTS `core_pack`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_pack` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `code` varchar(16) NOT NULL,
  `image` varchar(255) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_pack`
--

LOCK TABLES `core_pack` WRITE;
/*!40000 ALTER TABLE `core_pack` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_pack` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_pack_item`
--

DROP TABLE IF EXISTS `core_pack_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_pack_item` (
  `pack_id` int(10) unsigned NOT NULL DEFAULT '0',
  `model_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `record_id` int(11) NOT NULL,
  `position` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`pack_id`,`model_id`,`record_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_pack_item`
--

LOCK TABLES `core_pack_item` WRITE;
/*!40000 ALTER TABLE `core_pack_item` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_pack_item` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_page`
--

DROP TABLE IF EXISTS `core_page`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_page` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `about` tinytext NOT NULL,
  `app` varchar(64) NOT NULL,
  `code` varchar(64) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `image` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `thumbnail` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=245 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_page`
--

LOCK TABLES `core_page` WRITE;
/*!40000 ALTER TABLE `core_page` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_page` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_pagelet`
--

DROP TABLE IF EXISTS `core_pagelet`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_pagelet` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `code` varchar(32) NOT NULL,
  `content` text NOT NULL,
  `description` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_pagelet`
--

LOCK TABLES `core_pagelet` WRITE;
/*!40000 ALTER TABLE `core_pagelet` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_pagelet` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_paper`
--

DROP TABLE IF EXISTS `core_paper`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_paper` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `about` tinytext NOT NULL,
  `app` varchar(64) NOT NULL,
  `code` varchar(64) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_paper`
--

LOCK TABLES `core_paper` WRITE;
/*!40000 ALTER TABLE `core_paper` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_paper` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_paragraph`
--

DROP TABLE IF EXISTS `core_paragraph`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_paragraph` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `ancestor_id` int(10) unsigned NOT NULL DEFAULT '0',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `paper_id` int(10) unsigned NOT NULL DEFAULT '0',
  `path` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `position` smallint(5) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `ancestor_id` (`ancestor_id`),
  KEY `paper_id` (`paper_id`),
  KEY `path` (`path`),
  KEY `position` (`position`)
) ENGINE=MyISAM AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_paragraph`
--

LOCK TABLES `core_paragraph` WRITE;
/*!40000 ALTER TABLE `core_paragraph` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_paragraph` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_parameter`
--

DROP TABLE IF EXISTS `core_parameter`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_parameter` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `code` varchar(128) NOT NULL,
  `content` text NOT NULL,
  `description` varchar(255) NOT NULL,
  `name` varchar(128) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=85 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_parameter`
--

LOCK TABLES `core_parameter` WRITE;
/*!40000 ALTER TABLE `core_parameter` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_parameter` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_payment`
--

DROP TABLE IF EXISTS `core_payment`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_payment` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `amount` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `code` varchar(255) NOT NULL,
  `datas` text NOT NULL,
  `description` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `method_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `order_id` int(10) unsigned NOT NULL DEFAULT '0',
  `paid_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `placed_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `subscriber` varchar(255) NOT NULL COMMENT 'num abonné payboxdirect+',
  `token` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `method_id` (`method_id`)
) ENGINE=MyISAM AUTO_INCREMENT=1094 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_payment`
--

LOCK TABLES `core_payment` WRITE;
/*!40000 ALTER TABLE `core_payment` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_payment` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_person`
--

DROP TABLE IF EXISTS `core_person`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_person` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `address` varchar(255) NOT NULL,
  `address_extra` varchar(255) NOT NULL,
  `admin2` varchar(16) NOT NULL,
  `business` varchar(255) NOT NULL,
  `city` varchar(128) NOT NULL,
  `civility` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `country_id` smallint(5) unsigned NOT NULL DEFAULT '250',
  `description` varchar(255) NOT NULL,
  `fax` varchar(16) NOT NULL,
  `first_name` varchar(64) NOT NULL,
  `last_name` varchar(64) NOT NULL,
  `marital_status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `mobile` varchar(16) NOT NULL,
  `occupation` varchar(64) NOT NULL,
  `organization` varchar(255) NOT NULL,
  `phone` varchar(16) NOT NULL,
  `postal_code` varchar(32) NOT NULL,
  `region` varchar(64) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `twitter_id` varchar(64) NOT NULL,
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  `website` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `user_id` (`user_id`),
  KEY `last_name` (`last_name`)
) ENGINE=MyISAM AUTO_INCREMENT=14015 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_person`
--

LOCK TABLES `core_person` WRITE;
/*!40000 ALTER TABLE `core_person` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_person` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_ping`
--

DROP TABLE IF EXISTS `core_ping`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_ping` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `source_name` varchar(255) NOT NULL,
  `source_uri` varchar(255) NOT NULL,
  `received_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `item` (`model_id`,`record_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_ping`
--

LOCK TABLES `core_ping` WRITE;
/*!40000 ALTER TABLE `core_ping` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_ping` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_plugin`
--

DROP TABLE IF EXISTS `core_plugin`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_plugin` (
  `blog_id` int(10) unsigned NOT NULL DEFAULT '0',
  `plugin_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `args` text NOT NULL,
  `position` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`blog_id`,`plugin_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_plugin`
--

LOCK TABLES `core_plugin` WRITE;
/*!40000 ALTER TABLE `core_plugin` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_plugin` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_policy`
--

DROP TABLE IF EXISTS `core_policy`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_policy` (
  `id` tinyint(3) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `actions` text NOT NULL,
  `description` text NOT NULL,
  `extensions` varchar(255) NOT NULL,
  `name` varchar(32) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_policy`
--

LOCK TABLES `core_policy` WRITE;
/*!40000 ALTER TABLE `core_policy` DISABLE KEYS */;
INSERT INTO `core_policy` VALUES (1,'','','','','super administrateur');
/*!40000 ALTER TABLE `core_policy` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_poll`
--

DROP TABLE IF EXISTS `core_poll`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_poll` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(64) NOT NULL,
  `code` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `template` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_poll`
--

LOCK TABLES `core_poll` WRITE;
/*!40000 ALTER TABLE `core_poll` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_poll` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_popup`
--

DROP TABLE IF EXISTS `core_popup`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_popup` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(64) NOT NULL,
  `base_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `discount_id` int(10) unsigned NOT NULL DEFAULT '0',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `online_from` date NOT NULL DEFAULT '0000-00-00',
  `online_to` date NOT NULL DEFAULT '0000-00-00',
  `priority` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '-1',
  `visit_number` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `status` (`status`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_popup`
--

LOCK TABLES `core_popup` WRITE;
/*!40000 ALTER TABLE `core_popup` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_popup` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_popup_hit`
--

DROP TABLE IF EXISTS `core_popup_hit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_popup_hit` (
  `popup_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `ip` int(10) unsigned NOT NULL DEFAULT '0',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  `visit_id` int(10) unsigned NOT NULL DEFAULT '0',
  `opened_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `popup_id` (`popup_id`),
  KEY `user_id` (`user_id`,`visit_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_popup_hit`
--

LOCK TABLES `core_popup_hit` WRITE;
/*!40000 ALTER TABLE `core_popup_hit` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_popup_hit` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_post`
--

DROP TABLE IF EXISTS `core_post`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_post` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `author_id` int(10) unsigned NOT NULL DEFAULT '0',
  `blog_id` int(10) unsigned NOT NULL,
  `code` varchar(128) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `published_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `status` tinyint(4) NOT NULL DEFAULT '-50',
  PRIMARY KEY (`id`),
  KEY `blog_id` (`blog_id`),
  KEY `published_at` (`published_at`),
  KEY `code` (`code`),
  KEY `author_id` (`author_id`)
) ENGINE=MyISAM AUTO_INCREMENT=88 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_post`
--

LOCK TABLES `core_post` WRITE;
/*!40000 ALTER TABLE `core_post` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_post` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_privilege`
--

DROP TABLE IF EXISTS `core_privilege`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_privilege` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=15 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_privilege`
--

LOCK TABLES `core_privilege` WRITE;
/*!40000 ALTER TABLE `core_privilege` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_privilege` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_privilege_user`
--

DROP TABLE IF EXISTS `core_privilege_user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_privilege_user` (
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  `privilege_id` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `available_from` date NOT NULL DEFAULT '0000-00-00',
  `available_to` date NOT NULL DEFAULT '0000-00-00',
  PRIMARY KEY (`user_id`,`privilege_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_privilege_user`
--

LOCK TABLES `core_privilege_user` WRITE;
/*!40000 ALTER TABLE `core_privilege_user` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_privilege_user` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_process`
--

DROP TABLE IF EXISTS `core_process`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_process` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `execution_id` int(10) unsigned NOT NULL DEFAULT '0',
  `task_id` int(10) unsigned NOT NULL DEFAULT '0',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  `buffer_output` text NOT NULL,
  `completed_at` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `error` text NOT NULL,
  `input` text NOT NULL,
  `name` varchar(255) NOT NULL,
  `node_path` text NOT NULL,
  `output` text NOT NULL,
  `pid` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `priority` smallint(5) unsigned NOT NULL DEFAULT '0',
  `progress` text NOT NULL,
  `queued_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `scheduled_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `started_at` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `status` tinyint(3) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `execution_id` (`execution_id`),
  KEY `task_id` (`task_id`),
  KEY `model_id` (`model_id`),
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_process`
--

LOCK TABLES `core_process` WRITE;
/*!40000 ALTER TABLE `core_process` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_process` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_property`
--

DROP TABLE IF EXISTS `core_property`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_property` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `description` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `model_id` (`model_id`)
) ENGINE=MyISAM AUTO_INCREMENT=18021 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_property`
--

LOCK TABLES `core_property` WRITE;
/*!40000 ALTER TABLE `core_property` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_property` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_psp`
--

DROP TABLE IF EXISTS `core_psp`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_psp` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=12 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_psp`
--

LOCK TABLES `core_psp` WRITE;
/*!40000 ALTER TABLE `core_psp` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_psp` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_purchase`
--

DROP TABLE IF EXISTS `core_purchase`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_purchase` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `brand` varchar(255) NOT NULL,
  `category` varchar(64) NOT NULL,
  `code` varchar(255) NOT NULL,
  `custom1` varchar(64) NOT NULL,
  `custom2` varchar(64) NOT NULL,
  `custom3` varchar(64) NOT NULL,
  `custom4` varchar(64) NOT NULL,
  `custom5` varchar(64) NOT NULL,
  `description` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `order_id` int(10) unsigned NOT NULL DEFAULT '0',
  `original_price` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `pack_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `parcel_type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `price` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `purchased_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `quantity` mediumint(8) unsigned NOT NULL DEFAULT '1',
  `shipment_id` int(10) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `stock` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'état du stock avant achat',
  `tax` float(6,2) unsigned NOT NULL DEFAULT '0.00',
  `variations` text NOT NULL,
  PRIMARY KEY (`id`),
  KEY `item` (`model_id`,`record_id`),
  KEY `shipment_id` (`shipment_id`),
  KEY `order_id` (`order_id`)
) ENGINE=MyISAM AUTO_INCREMENT=10231 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_purchase`
--

LOCK TABLES `core_purchase` WRITE;
/*!40000 ALTER TABLE `core_purchase` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_purchase` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_redirection`
--

DROP TABLE IF EXISTS `core_redirection`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_redirection` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `destination` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_redirection`
--

LOCK TABLES `core_redirection` WRITE;
/*!40000 ALTER TABLE `core_redirection` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_redirection` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_region`
--

DROP TABLE IF EXISTS `core_region`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_region` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `content` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_region`
--

LOCK TABLES `core_region` WRITE;
/*!40000 ALTER TABLE `core_region` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_region` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_resource`
--

DROP TABLE IF EXISTS `core_resource`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_resource` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `crc` int(11) NOT NULL DEFAULT '0',
  `description` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `height` smallint(5) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `image_url` varchar(500) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `md5` varchar(32) NOT NULL,
  `name` varchar(255) NOT NULL,
  `path` varchar(255) NOT NULL,
  `position` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `size` int(10) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `thumbnail` varchar(255) NOT NULL,
  `thumbnail_url` varchar(500) NOT NULL,
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `url` varchar(500) NOT NULL,
  `width` smallint(5) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `item` (`model_id`,`record_id`)
) ENGINE=MyISAM AUTO_INCREMENT=219301 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_resource`
--

LOCK TABLES `core_resource` WRITE;
/*!40000 ALTER TABLE `core_resource` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_resource` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_search`
--

DROP TABLE IF EXISTS `core_search`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_search` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `crawler` varchar(255) NOT NULL,
  `crc` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'options',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `found_rows` int(10) unsigned NOT NULL DEFAULT '0',
  `keyword` varchar(255) NOT NULL,
  `latitude` double NOT NULL,
  `longitude` double NOT NULL,
  `metas` text NOT NULL,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `opts` text NOT NULL COMMENT 'no offset, limit',
  `radius` smallint(5) unsigned NOT NULL DEFAULT '0',
  `refreshed_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `stored_rows` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `crc` (`crc`)
) ENGINE=MyISAM AUTO_INCREMENT=312 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_search`
--

LOCK TABLES `core_search` WRITE;
/*!40000 ALTER TABLE `core_search` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_search` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_search_results`
--

DROP TABLE IF EXISTS `core_search_results`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_search_results` (
  `search_id` int(10) unsigned NOT NULL DEFAULT '0',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `position` int(10) unsigned NOT NULL DEFAULT '0',
  KEY `search_id` (`search_id`),
  KEY `record_id` (`record_id`),
  KEY `position` (`position`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_search_results`
--

LOCK TABLES `core_search_results` WRITE;
/*!40000 ALTER TABLE `core_search_results` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_search_results` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_search_updates`
--

DROP TABLE IF EXISTS `core_search_updates`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_search_updates` (
  `search_id` int(10) unsigned NOT NULL DEFAULT '0',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `added_on` date NOT NULL DEFAULT '0000-00-00',
  PRIMARY KEY (`search_id`,`model_id`,`record_id`),
  KEY `added_on` (`added_on`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_search_updates`
--

LOCK TABLES `core_search_updates` WRITE;
/*!40000 ALTER TABLE `core_search_updates` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_search_updates` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_section`
--

DROP TABLE IF EXISTS `core_section`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_section` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `faq_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `position` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  KEY `faq_id` (`faq_id`)
) ENGINE=MyISAM AUTO_INCREMENT=27 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_section`
--

LOCK TABLES `core_section` WRITE;
/*!40000 ALTER TABLE `core_section` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_section` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_shipment`
--

DROP TABLE IF EXISTS `core_shipment`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_shipment` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `amount` float(9,2) unsigned NOT NULL DEFAULT '0.00',
  `carrier_id` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `order_id` int(10) unsigned NOT NULL DEFAULT '0',
  `parcel_type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `tracking_number` varchar(32) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `order_id` (`order_id`)
) ENGINE=MyISAM AUTO_INCREMENT=1075 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_shipment`
--

LOCK TABLES `core_shipment` WRITE;
/*!40000 ALTER TABLE `core_shipment` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_shipment` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_slide`
--

DROP TABLE IF EXISTS `core_slide`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_slide` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `online_from` date NOT NULL DEFAULT '0000-00-00',
  `online_to` date NOT NULL DEFAULT '0000-00-00',
  `position` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `slideshow_id` int(10) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `status` (`status`)
) ENGINE=MyISAM AUTO_INCREMENT=39 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_slide`
--

LOCK TABLES `core_slide` WRITE;
/*!40000 ALTER TABLE `core_slide` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_slide` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_slideshow`
--

DROP TABLE IF EXISTS `core_slideshow`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_slideshow` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(16) NOT NULL,
  `code` varchar(64) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `template` varchar(128) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_slideshow`
--

LOCK TABLES `core_slideshow` WRITE;
/*!40000 ALTER TABLE `core_slideshow` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_slideshow` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_smtp`
--

DROP TABLE IF EXISTS `core_smtp`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_smtp` (
  `email_id` int(10) unsigned NOT NULL DEFAULT '0',
  `campaign_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `smtp_key` varchar(16) NOT NULL,
  `relay_host` varchar(255) NOT NULL,
  `relay_ip` varchar(15) NOT NULL,
  `stat` varchar(255) NOT NULL,
  `dsn` varchar(5) NOT NULL,
  `from_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `to_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `dsn` (`dsn`),
  KEY `campaign_id` (`campaign_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_smtp`
--

LOCK TABLES `core_smtp` WRITE;
/*!40000 ALTER TABLE `core_smtp` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_smtp` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_snippet`
--

DROP TABLE IF EXISTS `core_snippet`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_snippet` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `about` tinytext NOT NULL,
  `app` varchar(32) NOT NULL,
  `code` varchar(128) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '20',
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=451 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_snippet`
--

LOCK TABLES `core_snippet` WRITE;
/*!40000 ALTER TABLE `core_snippet` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_snippet` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_space`
--

DROP TABLE IF EXISTS `core_space`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_space` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(16) NOT NULL,
  `name` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_space`
--

LOCK TABLES `core_space` WRITE;
/*!40000 ALTER TABLE `core_space` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_space` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_sponsorship`
--

DROP TABLE IF EXISTS `core_sponsorship`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_sponsorship` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `content` text NOT NULL,
  `email_id` int(10) unsigned NOT NULL DEFAULT '0',
  `godson_id` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`),
  KEY `godson_id` (`godson_id`)
) ENGINE=MyISAM AUTO_INCREMENT=45 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_sponsorship`
--

LOCK TABLES `core_sponsorship` WRITE;
/*!40000 ALTER TABLE `core_sponsorship` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_sponsorship` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_stem`
--

DROP TABLE IF EXISTS `core_stem`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_stem` (
  `id` mediumint(8) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(32) CHARACTER SET utf8 NOT NULL,
  PRIMARY KEY (`id`),
  KEY `name` (`name`(3))
) ENGINE=MyISAM AUTO_INCREMENT=56840 DEFAULT CHARSET=utf8 COLLATE=utf8_bin ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_stem`
--

LOCK TABLES `core_stem` WRITE;
/*!40000 ALTER TABLE `core_stem` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_stem` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_story`
--

DROP TABLE IF EXISTS `core_story`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_story` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `about` tinytext NOT NULL,
  `app` varchar(64) NOT NULL,
  `author_id` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `code` varchar(64) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `published_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `scheduled_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `status` tinyint(4) NOT NULL DEFAULT '-50',
  `thumbnail` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`),
  KEY `author_id` (`author_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_story`
--

LOCK TABLES `core_story` WRITE;
/*!40000 ALTER TABLE `core_story` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_story` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_stream`
--

DROP TABLE IF EXISTS `core_stream`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_stream` (
  `model_id` smallint(6) unsigned NOT NULL DEFAULT '0',
  `record_id` int(11) unsigned NOT NULL DEFAULT '0',
  `user_id` int(11) unsigned NOT NULL DEFAULT '0',
  `type` tinyint(4) unsigned NOT NULL DEFAULT '0',
  `datas` text NOT NULL,
  `created_on` date NOT NULL DEFAULT '0000-00-00',
  `created_at` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  KEY `item` (`model_id`,`record_id`),
  KEY `user_id` (`user_id`),
  KEY `type` (`type`),
  KEY `created_on` (`created_on`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_stream`
--

LOCK TABLES `core_stream` WRITE;
/*!40000 ALTER TABLE `core_stream` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_stream` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_string`
--

DROP TABLE IF EXISTS `core_string`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_string` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `about` tinytext NOT NULL,
  `app` varchar(64) NOT NULL,
  `code` varchar(128) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '20',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `used_at` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`id`),
  UNIQUE KEY `code_2` (`code`,`type`),
  KEY `type` (`type`),
  KEY `code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=8968 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_string`
--

LOCK TABLES `core_string` WRITE;
/*!40000 ALTER TABLE `core_string` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_string` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_stylesheet`
--

DROP TABLE IF EXISTS `core_stylesheet`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_stylesheet` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `align` varchar(8) NOT NULL,
  `app` varchar(32) NOT NULL,
  `background` varchar(128) NOT NULL,
  `border` varchar(128) NOT NULL,
  `code` varchar(16) NOT NULL,
  `color` varchar(8) NOT NULL,
  `decoration` varchar(8) NOT NULL,
  `extra` varchar(255) NOT NULL,
  `font` varchar(32) NOT NULL,
  `letter_spacing` varchar(8) NOT NULL,
  `line_height` varchar(8) NOT NULL,
  `margin` varchar(16) NOT NULL,
  `name` varchar(32) NOT NULL,
  `padding` varchar(16) NOT NULL,
  `size` varchar(8) NOT NULL,
  `style` varchar(16) NOT NULL,
  `tag_type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `transform` varchar(16) NOT NULL,
  `weight` varchar(8) NOT NULL,
  `word_spacing` varchar(8) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=11 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_stylesheet`
--

LOCK TABLES `core_stylesheet` WRITE;
/*!40000 ALTER TABLE `core_stylesheet` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_stylesheet` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_submission`
--

DROP TABLE IF EXISTS `core_submission`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_submission` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `form_id` smallint(3) unsigned NOT NULL DEFAULT '0',
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `submitted_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `form` (`form_id`,`status`)
) ENGINE=MyISAM AUTO_INCREMENT=432 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_submission`
--

LOCK TABLES `core_submission` WRITE;
/*!40000 ALTER TABLE `core_submission` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_submission` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_subscription`
--

DROP TABLE IF EXISTS `core_subscription`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_subscription` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `email_id` int(10) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `email_id` (`email_id`),
  KEY `item` (`model_id`,`record_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_subscription`
--

LOCK TABLES `core_subscription` WRITE;
/*!40000 ALTER TABLE `core_subscription` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_subscription` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_suggestion`
--

DROP TABLE IF EXISTS `core_suggestion`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_suggestion` (
  `id` mediumint(8) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `attributes` text NOT NULL,
  `content` text NOT NULL,
  `label` text NOT NULL,
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `item` (`model_id`,`record_id`,`locale`)
) ENGINE=MyISAM AUTO_INCREMENT=444 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_suggestion`
--

LOCK TABLES `core_suggestion` WRITE;
/*!40000 ALTER TABLE `core_suggestion` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_suggestion` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_suggestion_hit`
--

DROP TABLE IF EXISTS `core_suggestion_hit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_suggestion_hit` (
  `keyword` varchar(255) NOT NULL,
  `suggestion_id` int(10) unsigned NOT NULL DEFAULT '0',
  `visit_id` int(10) unsigned NOT NULL DEFAULT '0',
  `inserted_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `suggestion_id` (`suggestion_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_suggestion_hit`
--

LOCK TABLES `core_suggestion_hit` WRITE;
/*!40000 ALTER TABLE `core_suggestion_hit` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_suggestion_hit` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_supervisor`
--

DROP TABLE IF EXISTS `core_supervisor`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_supervisor` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `content` text NOT NULL,
  `description` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `frequency` smallint(5) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_supervisor`
--

LOCK TABLES `core_supervisor` WRITE;
/*!40000 ALTER TABLE `core_supervisor` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_supervisor` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_survey`
--

DROP TABLE IF EXISTS `core_survey`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_survey` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `content` text NOT NULL,
  `description` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_survey`
--

LOCK TABLES `core_survey` WRITE;
/*!40000 ALTER TABLE `core_survey` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_survey` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_synonym`
--

DROP TABLE IF EXISTS `core_synonym`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_synonym` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `synonym` varchar(128) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_synonym`
--

LOCK TABLES `core_synonym` WRITE;
/*!40000 ALTER TABLE `core_synonym` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_synonym` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_tag`
--

DROP TABLE IF EXISTS `core_tag`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_tag` (
  `id` mediumint(8) unsigned NOT NULL AUTO_INCREMENT,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(48) COLLATE utf8_bin NOT NULL,
  `status` tinyint(1) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM AUTO_INCREMENT=2305 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_tag`
--

LOCK TABLES `core_tag` WRITE;
/*!40000 ALTER TABLE `core_tag` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_tag` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_task`
--

DROP TABLE IF EXISTS `core_task`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_task` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `code` varchar(255) NOT NULL,
  `inputs` text NOT NULL COMMENT 'les variables attendues en arguments',
  `inputs_user` text NOT NULL,
  `method` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `outputs` text NOT NULL COMMENT 'les variables retournées',
  `params` text NOT NULL COMMENT 'des paramètres nécessaire au Process (ex. url du endpoint Medianate, framerate pour ffmpeg)',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=14 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_task`
--

LOCK TABLES `core_task` WRITE;
/*!40000 ALTER TABLE `core_task` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_task` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_term`
--

DROP TABLE IF EXISTS `core_term`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_term` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `glossary_id` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  KEY `name` (`name`),
  KEY `glossary_id` (`glossary_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_term`
--

LOCK TABLES `core_term` WRITE;
/*!40000 ALTER TABLE `core_term` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_term` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_timezone`
--

DROP TABLE IF EXISTS `core_timezone`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_timezone` (
  `id` tinyint(3) unsigned NOT NULL AUTO_INCREMENT,
  `code` varchar(32) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `utc` char(6) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=144 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_timezone`
--

LOCK TABLES `core_timezone` WRITE;
/*!40000 ALTER TABLE `core_timezone` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_timezone` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_topic`
--

DROP TABLE IF EXISTS `core_topic`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_topic` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `content` text NOT NULL,
  `forum_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `posted_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`),
  KEY `forum_id_2` (`forum_id`)
) ENGINE=MyISAM AUTO_INCREMENT=32 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_topic`
--

LOCK TABLES `core_topic` WRITE;
/*!40000 ALTER TABLE `core_topic` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_topic` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_tracker_expression`
--

DROP TABLE IF EXISTS `core_tracker_expression`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_tracker_expression` (
  `id` mediumint(8) unsigned NOT NULL AUTO_INCREMENT,
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) COLLATE utf8_bin NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `string` (`name`,`type`)
) ENGINE=MyISAM AUTO_INCREMENT=10972 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_tracker_expression`
--

LOCK TABLES `core_tracker_expression` WRITE;
/*!40000 ALTER TABLE `core_tracker_expression` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_tracker_expression` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_tracker_hit`
--

DROP TABLE IF EXISTS `core_tracker_hit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_tracker_hit` (
  `visit_id` int(10) unsigned NOT NULL DEFAULT '0',
  `action_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `category_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `visit_id` (`visit_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_tracker_hit`
--

LOCK TABLES `core_tracker_hit` WRITE;
/*!40000 ALTER TABLE `core_tracker_hit` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_tracker_hit` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_tracker_hit_tmp`
--

DROP TABLE IF EXISTS `core_tracker_hit_tmp`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_tracker_hit_tmp` (
  `visit_id` int(10) unsigned NOT NULL,
  `action` varchar(255) NOT NULL,
  `category` varchar(128) NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `visit_id` (`visit_id`),
  KEY `created_at` (`created_at`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_tracker_hit_tmp`
--

LOCK TABLES `core_tracker_hit_tmp` WRITE;
/*!40000 ALTER TABLE `core_tracker_hit_tmp` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_tracker_hit_tmp` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_tracker_report`
--

DROP TABLE IF EXISTS `core_tracker_report`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_tracker_report` (
  `year` smallint(5) unsigned NOT NULL DEFAULT '0',
  `month` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `week` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `day` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `browser` text NOT NULL,
  `campaign` text NOT NULL,
  `community` text NOT NULL,
  `conversion` text NOT NULL,
  `country` text NOT NULL,
  `depth` text NOT NULL,
  `event` text NOT NULL,
  `exit` text NOT NULL,
  `host` text NOT NULL,
  `hour` text NOT NULL,
  `interaction` text NOT NULL,
  `landing` text NOT NULL,
  `locale` text NOT NULL,
  `medium` text NOT NULL,
  `metric` text NOT NULL,
  `path` text NOT NULL,
  `search` text NOT NULL,
  `share` text NOT NULL,
  `source` text NOT NULL,
  `system` text NOT NULL,
  `term` text NOT NULL,
  `date` date NOT NULL,
  KEY `histo_key` (`year`,`month`,`week`,`day`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_tracker_report`
--

LOCK TABLES `core_tracker_report` WRITE;
/*!40000 ALTER TABLE `core_tracker_report` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_tracker_report` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_tracker_robot`
--

DROP TABLE IF EXISTS `core_tracker_robot`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_tracker_robot` (
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `uri` varchar(255) DEFAULT NULL,
  `robot` varchar(255) DEFAULT NULL,
  `address` varchar(15) NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_tracker_robot`
--

LOCK TABLES `core_tracker_robot` WRITE;
/*!40000 ALTER TABLE `core_tracker_robot` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_tracker_robot` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_tracker_search`
--

DROP TABLE IF EXISTS `core_tracker_search`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_tracker_search` (
  `query_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `visit_id` int(10) unsigned NOT NULL DEFAULT '0',
  `found_rows` smallint(5) unsigned NOT NULL DEFAULT '0',
  `created_on` date NOT NULL DEFAULT '0000-00-00',
  PRIMARY KEY (`visit_id`,`query_id`),
  KEY `created_on` (`created_on`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_tracker_search`
--

LOCK TABLES `core_tracker_search` WRITE;
/*!40000 ALTER TABLE `core_tracker_search` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_tracker_search` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_tracker_visit`
--

DROP TABLE IF EXISTS `core_tracker_visit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_tracker_visit` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  `visitor_id` int(10) unsigned NOT NULL DEFAULT '0',
  `browser` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `campaign` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `conversions` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `country` smallint(5) unsigned NOT NULL DEFAULT '0',
  `depth` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `duration` smallint(5) unsigned NOT NULL DEFAULT '0',
  `ip` int(10) unsigned NOT NULL DEFAULT '0',
  `layout` smallint(5) unsigned NOT NULL DEFAULT '0',
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `medium` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `number` smallint(5) unsigned NOT NULL DEFAULT '1',
  `path` int(10) unsigned NOT NULL DEFAULT '0',
  `source` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `span` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `system` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `term` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `url_id` int(10) unsigned NOT NULL DEFAULT '0',
  `created_on` date NOT NULL DEFAULT '0000-00-00',
  `created_at` time NOT NULL DEFAULT '00:00:00',
  PRIMARY KEY (`id`),
  KEY `created_on` (`created_on`),
  KEY `visitor_id` (`visitor_id`)
) ENGINE=MyISAM AUTO_INCREMENT=68730 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_tracker_visit`
--

LOCK TABLES `core_tracker_visit` WRITE;
/*!40000 ALTER TABLE `core_tracker_visit` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_tracker_visit` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_tracker_visitor`
--

DROP TABLE IF EXISTS `core_tracker_visitor`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_tracker_visitor` (
  `visitor_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `visit_id` int(10) unsigned NOT NULL DEFAULT '0',
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `crc` int(10) unsigned NOT NULL DEFAULT '0',
  `ip` int(10) unsigned NOT NULL DEFAULT '0',
  `counter` smallint(5) unsigned NOT NULL DEFAULT '0',
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`visitor_id`),
  KEY `ip` (`ip`),
  KEY `created_at` (`created_at`)
) ENGINE=MyISAM AUTO_INCREMENT=353229 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_tracker_visitor`
--

LOCK TABLES `core_tracker_visitor` WRITE;
/*!40000 ALTER TABLE `core_tracker_visitor` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_tracker_visitor` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_trackvar`
--

DROP TABLE IF EXISTS `core_trackvar`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_trackvar` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(32) NOT NULL,
  `campaign` varchar(255) NOT NULL,
  `cost` float(5,2) unsigned NOT NULL DEFAULT '0.00',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `medium` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `name` varchar(255) NOT NULL,
  `source` varchar(255) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  `term` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_trackvar`
--

LOCK TABLES `core_trackvar` WRITE;
/*!40000 ALTER TABLE `core_trackvar` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_trackvar` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_transaction`
--

DROP TABLE IF EXISTS `core_transaction`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_transaction` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `amount` float(8,2) unsigned NOT NULL,
  `customer` text NOT NULL,
  `description` text NOT NULL,
  `parameters` text NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_transaction`
--

LOCK TABLES `core_transaction` WRITE;
/*!40000 ALTER TABLE `core_transaction` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_transaction` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_translation`
--

DROP TABLE IF EXISTS `core_translation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_translation` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `content` text NOT NULL COMMENT 'dialog',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `properties` mediumtext NOT NULL,
  `priority` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `item` (`model_id`,`record_id`,`locale`)
) ENGINE=MyISAM AUTO_INCREMENT=20 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_translation`
--

LOCK TABLES `core_translation` WRITE;
/*!40000 ALTER TABLE `core_translation` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_translation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_translator`
--

DROP TABLE IF EXISTS `core_translator`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_translator` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `content` text NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `image` varchar(255) NOT NULL,
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_translator`
--

LOCK TABLES `core_translator` WRITE;
/*!40000 ALTER TABLE `core_translator` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_translator` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_tree`
--

DROP TABLE IF EXISTS `core_tree`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_tree` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(16) NOT NULL,
  `build_method` varchar(255) NOT NULL,
  `code` varchar(32) NOT NULL,
  `columns` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `height` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `height_max` tinyint(3) unsigned NOT NULL DEFAULT '8',
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `models` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_tree`
--

LOCK TABLES `core_tree` WRITE;
/*!40000 ALTER TABLE `core_tree` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_tree` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_url`
--

DROP TABLE IF EXISTS `core_url`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_url` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `crc` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(1024) COLLATE utf8_bin NOT NULL,
  PRIMARY KEY (`id`),
  KEY `crc` (`crc`)
) ENGINE=MyISAM AUTO_INCREMENT=2370 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_url`
--

LOCK TABLES `core_url` WRITE;
/*!40000 ALTER TABLE `core_url` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_url` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_user`
--

DROP TABLE IF EXISTS `core_user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_user` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `complaints` int(10) unsigned NOT NULL DEFAULT '0',
  `email` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locale` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(128) NOT NULL,
  `notices` int(10) unsigned NOT NULL DEFAULT '0',
  `password` varchar(32) NOT NULL,
  `path` varchar(255) NOT NULL,
  `privacy` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `privileges` int(10) unsigned NOT NULL DEFAULT '0',
  `profiles` int(10) unsigned NOT NULL DEFAULT '0',
  `registered_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  KEY `email` (`email`(6)),
  KEY `name` (`name`),
  KEY `notices` (`notices`),
  KEY `privileges` (`privileges`)
) ENGINE=MyISAM AUTO_INCREMENT=15860 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_user`
--

LOCK TABLES `core_user` WRITE;
/*!40000 ALTER TABLE `core_user` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_user` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_user_information`
--

DROP TABLE IF EXISTS `core_user_information`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_user_information` (
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  `action_count` smallint(5) unsigned NOT NULL DEFAULT '0',
  `auth_at` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `auth_count` smallint(5) unsigned NOT NULL DEFAULT '1',
  `avatar_url` varchar(255) NOT NULL,
  `birth_date` date NOT NULL DEFAULT '0000-00-00',
  `headline` varchar(140) NOT NULL,
  `last_order_date` date NOT NULL DEFAULT '0000-00-00',
  `origin_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `points` smallint(5) unsigned NOT NULL DEFAULT '0',
  `popularity` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `quota_hard` smallint(5) unsigned NOT NULL DEFAULT '50',
  `quota_soft` smallint(5) unsigned NOT NULL DEFAULT '200',
  `recipient_id` int(10) unsigned NOT NULL DEFAULT '0',
  `timezone_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`user_id`),
  KEY `recipient_id` (`recipient_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_user_information`
--

LOCK TABLES `core_user_information` WRITE;
/*!40000 ALTER TABLE `core_user_information` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_user_information` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_valuation`
--

DROP TABLE IF EXISTS `core_valuation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_valuation` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app_id` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `model_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `record_id` int(10) unsigned NOT NULL DEFAULT '0',
  `comment` text NOT NULL,
  `grade` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `opinion` tinyint(4) NOT NULL DEFAULT '0',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `item` (`model_id`,`record_id`)
) ENGINE=MyISAM AUTO_INCREMENT=31 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_valuation`
--

LOCK TABLES `core_valuation` WRITE;
/*!40000 ALTER TABLE `core_valuation` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_valuation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_warehouse`
--

DROP TABLE IF EXISTS `core_warehouse`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_warehouse` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(64) NOT NULL,
  `address` varchar(255) NOT NULL,
  `amount` float(6,2) unsigned NOT NULL DEFAULT '0.00',
  `carrier_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `country_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `locales` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `postal_code` varchar(8) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_warehouse`
--

LOCK TABLES `core_warehouse` WRITE;
/*!40000 ALTER TABLE `core_warehouse` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_warehouse` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `core_workflow`
--

DROP TABLE IF EXISTS `core_workflow`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `core_workflow` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `code` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `tasks` text NOT NULL COMMENT '(structure enrichie avec l''attribut path : ex. aa aaab aaac)',
  `tasks_user` text NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `core_workflow`
--

LOCK TABLES `core_workflow` WRITE;
/*!40000 ALTER TABLE `core_workflow` DISABLE KEYS */;
/*!40000 ALTER TABLE `core_workflow` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `knx_admin1`
--

DROP TABLE IF EXISTS `knx_admin1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `knx_admin1` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `code` int(10) unsigned NOT NULL DEFAULT '0',
  `country_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`country_id`,`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `knx_admin1`
--

LOCK TABLES `knx_admin1` WRITE;
/*!40000 ALTER TABLE `knx_admin1` DISABLE KEYS */;
/*!40000 ALTER TABLE `knx_admin1` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `knx_admin2`
--

DROP TABLE IF EXISTS `knx_admin2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `knx_admin2` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `admin1_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `code` int(10) unsigned NOT NULL COMMENT 'sprintf(''%u'', crc32($code))',
  `country_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `admin1_id` (`admin1_id`),
  KEY `country_id` (`country_id`),
  KEY `code` (`code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `knx_admin2`
--

LOCK TABLES `knx_admin2` WRITE;
/*!40000 ALTER TABLE `knx_admin2` DISABLE KEYS */;
/*!40000 ALTER TABLE `knx_admin2` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `knx_business_naf`
--

DROP TABLE IF EXISTS `knx_business_naf`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `knx_business_naf` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `code` varchar(4) NOT NULL,
  `business_id` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `business_code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=717 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `knx_business_naf`
--

LOCK TABLES `knx_business_naf` WRITE;
/*!40000 ALTER TABLE `knx_business_naf` DISABLE KEYS */;
/*!40000 ALTER TABLE `knx_business_naf` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `knx_city`
--

DROP TABLE IF EXISTS `knx_city`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `knx_city` (
  `id` mediumint(8) unsigned NOT NULL,
  `admin1_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `admin2_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `country_id` smallint(5) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL,
  `population` int(10) unsigned NOT NULL DEFAULT '0',
  `latitude` double NOT NULL DEFAULT '0',
  `longitude` double NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `city` (`name`,`admin1_id`,`country_id`),
  KEY `admin1_id` (`admin1_id`),
  KEY `admin2_id` (`admin2_id`),
  KEY `country_id` (`country_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `knx_city`
--

LOCK TABLES `knx_city` WRITE;
/*!40000 ALTER TABLE `knx_city` DISABLE KEYS */;
/*!40000 ALTER TABLE `knx_city` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `knx_city_ip`
--

DROP TABLE IF EXISTS `knx_city_ip`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `knx_city_ip` (
  `city_id` int(11) NOT NULL,
  `from_ip` int(10) unsigned NOT NULL DEFAULT '0',
  `to_ip` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`from_ip`,`to_ip`),
  KEY `city_id` (`city_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `knx_city_ip`
--

LOCK TABLES `knx_city_ip` WRITE;
/*!40000 ALTER TABLE `knx_city_ip` DISABLE KEYS */;
/*!40000 ALTER TABLE `knx_city_ip` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `knx_continent`
--

DROP TABLE IF EXISTS `knx_continent`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `knx_continent` (
  `id` mediumint(8) unsigned NOT NULL,
  `code` varchar(2) NOT NULL,
  `name_fr` varchar(255) NOT NULL,
  `name_en` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `knx_continent`
--

LOCK TABLES `knx_continent` WRITE;
/*!40000 ALTER TABLE `knx_continent` DISABLE KEYS */;
INSERT INTO `knx_continent` VALUES (6255146,'AF','Afrique','Africa'),(6255147,'AS','Asie','Asia'),(6255148,'EU','Europe','Europe'),(6255149,'NA','Amérique du nord','North America'),(6255151,'OC','Océanie','Oceania'),(6255150,'SA','Amérique du sud','South America'),(6255152,'AN','Antarctique','Antarctica');
/*!40000 ALTER TABLE `knx_continent` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `knx_firstname`
--

DROP TABLE IF EXISTS `knx_firstname`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `knx_firstname` (
  `gender` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `first_name` varchar(32) DEFAULT NULL,
  `ambiguous` tinyint(1) unsigned NOT NULL DEFAULT '0',
  UNIQUE KEY `first_name` (`first_name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `knx_firstname`
--

LOCK TABLES `knx_firstname` WRITE;
/*!40000 ALTER TABLE `knx_firstname` DISABLE KEYS */;
/*!40000 ALTER TABLE `knx_firstname` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `knx_language`
--

DROP TABLE IF EXISTS `knx_language`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `knx_language` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `status` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `code` varchar(2) NOT NULL COMMENT 'ISO 639-1',
  `code2` varchar(3) NOT NULL COMMENT 'ISO 639-2 B',
  `code3` varchar(3) NOT NULL COMMENT 'ISO 639-2 T',
  `name` varchar(255) NOT NULL,
  `native_name` varchar(255) NOT NULL,
  `num` smallint(5) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=306 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `knx_language`
--

LOCK TABLES `knx_language` WRITE;
/*!40000 ALTER TABLE `knx_language` DISABLE KEYS */;
/*!40000 ALTER TABLE `knx_language` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `test_building`
--

DROP TABLE IF EXISTS `test_building`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `test_building` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `residence_id` int(10) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `residence_id` (`residence_id`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `test_building`
--

LOCK TABLES `test_building` WRITE;
/*!40000 ALTER TABLE `test_building` DISABLE KEYS */;
INSERT INTO `test_building` VALUES (1,0,'Bâtiment A',1,1),(2,0,'Bâtiment B',1,1),(3,0,'Bâtiment 1',3,1);
/*!40000 ALTER TABLE `test_building` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `test_residence`
--

DROP TABLE IF EXISTS `test_residence`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `test_residence` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `address` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `address_extra` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `city` varchar(128) COLLATE utf8mb4_unicode_ci NOT NULL,
  `country_id` int(10) unsigned NOT NULL DEFAULT '0',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `postal_code` varchar(32) COLLATE utf8mb4_unicode_ci NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `country_id` (`country_id`)
) ENGINE=MyISAM AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `test_residence`
--

LOCK TABLES `test_residence` WRITE;
/*!40000 ALTER TABLE `test_residence` DISABLE KEYS */;
INSERT INTO `test_residence` VALUES (1,'16 rue Wenceslas Coutellier','','Clermont',250,0,'Résidence de la Croix St-Laurent','60600',1),(2,'15 rue Cels','','Paris',250,0,'Résidence des Lilas','75014',1),(3,'6 rue Lalande','','Paris',250,0,'Résidence des Roses','75014',1);
/*!40000 ALTER TABLE `test_residence` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `test_settlement`
--

DROP TABLE IF EXISTS `test_settlement`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `test_settlement` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `building_id` int(10) unsigned NOT NULL DEFAULT '0',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `residence_id` int(10) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `building_id` (`building_id`,`residence_id`)
) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `test_settlement`
--

LOCK TABLES `test_settlement` WRITE;
/*!40000 ALTER TABLE `test_settlement` DISABLE KEYS */;
INSERT INTO `test_settlement` VALUES (1,1,0,'Appartement 1',1,1),(2,1,1,'Appartement 2',1,1),(3,1,0,'Appartement 3',1,1),(4,2,0,'Appartement 1 bis',1,1),(5,0,0,'Loft',2,1),(6,3,0,'Bureaux',3,1);
/*!40000 ALTER TABLE `test_settlement` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2018-02-20 15:29:14
