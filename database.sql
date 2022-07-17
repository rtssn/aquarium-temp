
CREATE DATABASE IF NOT EXISTS `aquarium_temp`;

CREATE TABLE `temp` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `datetime` datetime NOT NULL,
  `sensor1Temp` float NOT NULL,
  `sensor2Temp` float NOT NULL,
  `isFanOn` boolean NOT NULL,
  `ipAddress` varchar(16) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `datetime` (`datetime`)
);