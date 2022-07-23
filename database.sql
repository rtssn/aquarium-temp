
CREATE DATABASE IF NOT EXISTS `aquarium_temp`;

USE `aquarium_temp`;

CREATE TABLE `temp` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `deviceId` varchar(32) NOT NULL,
  `datetime` datetime NOT NULL,
  `sensor1Temp` float NOT NULL,
  `sensor2Temp` float NOT NULL,
  `isFanOn` boolean NOT NULL,
  `ipAddress` varchar(16) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `datetime` (`datetime`),
  KEY `deviceId` (`deviceId`)
);

ALTER TABLE `temp` ADD `deviceId` varchar(32) NOT NULL;
ALTER TABLE `temp` ADD KEY (`deviceId`);