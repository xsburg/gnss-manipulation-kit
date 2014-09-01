delimiter $$

DROP TABLE IF EXISTS `commandqueue`;
DROP TABLE IF EXISTS `servicelog`;
DROP TABLE IF EXISTS `commandclassifier`;
DROP TABLE IF EXISTS `servicelogseverityclassifier`;
DROP TABLE IF EXISTS `configinfo`;
DROP TABLE IF EXISTS `status`;
DROP TABLE IF EXISTS `messageStatistics`;


CREATE TABLE `messageStatistics` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `messageId` VARCHAR(50) NOT NULL UNIQUE,
  `received` INT NOT NULL DEFAULT 0,
  `averageSize` FLOAT NOT NULL DEFAULT 0,
  `startedAt` DATETIME NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `status` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(64) NOT NULL UNIQUE,
  `value` varchar(1024) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `commandclassifier` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(100) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `name_UNIQUE` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;


CREATE TABLE `commandqueue` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `command_id` int(11) NOT NULL,
  `arguments` varchar(1000) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `commandidfk_idx` (`command_id`),
  CONSTRAINT `commandfk` FOREIGN KEY (`command_id`) REFERENCES `commandclassifier` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=14 DEFAULT CHARSET=utf8;


CREATE TABLE `configinfo` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(64) NOT NULL,
  `value` varchar(512) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `servicelogseverityclassifier` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;


CREATE TABLE `servicelog` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `timeStamp` datetime NOT NULL,
  `message` varchar(512) DEFAULT NULL,
  `type` int(11) NOT NULL,
  `severity_id` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `serviceidfk_idx` (`severity_id`),
  CONSTRAINT `serviceidfk` FOREIGN KEY (`severity_id`) REFERENCES `servicelogseverityclassifier` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

INSERT INTO `commandclassifier` (`id`, `name`) VALUES (1, 'RESTART');
INSERT INTO `commandclassifier` (`id`, `name`) VALUES (2, 'SETCONFIGPROPERTIES');
INSERT INTO `commandclassifier` (`id`, `name`) VALUES (3, 'SHUTDOWN');
INSERT INTO `commandclassifier` (`id`, `name`) VALUES (4, 'PAUSE');
INSERT INTO `commandclassifier` (`id`, `name`) VALUES (5, 'RESUME');
INSERT INTO `commandclassifier` (`id`, `name`) VALUES (6, 'UPDATECONFIGINFO');

INSERT INTO `servicelogseverityclassifier` (`id`, `name`) VALUES (1, 'FATAL');
INSERT INTO `servicelogseverityclassifier` (`id`, `name`) VALUES (2, 'ERROR');
INSERT INTO `servicelogseverityclassifier` (`id`, `name`) VALUES (3, 'WARNING');
INSERT INTO `servicelogseverityclassifier` (`id`, `name`) VALUES (4, 'INFO');
INSERT INTO `servicelogseverityclassifier` (`id`, `name`) VALUES (5, 'DEBUG');
