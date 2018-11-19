-- phpMyAdmin SQL Dump
-- version 4.0.10.20
-- https://www.phpmyadmin.net
--
-- Хост: 10.0.0.109:3306
-- Время создания: Ноя 19 2018 г., 20:37
-- Версия сервера: 10.1.37-MariaDB
-- Версия PHP: 5.3.3

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- База данных: `j775420_seospiderstatisticstest`
--

-- --------------------------------------------------------

--
-- Дублирующая структура для представления `all_users`
--
CREATE TABLE IF NOT EXISTS `all_users` (
`UserName` varchar(191)
,`OS` varchar(191)
,`Program version` varchar(191)
,`Program bittnes` varchar(191)
,`Country` varchar(191)
,`Locale language` varchar(191)
);
-- --------------------------------------------------------

--
-- Структура таблицы `counters`
--

CREATE TABLE IF NOT EXISTS `counters` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `CounterName` varchar(255) NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `CounterName_UNIQUE` (`CounterName`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=834 ;

-- --------------------------------------------------------

--
-- Структура таблицы `counters_data_by_session_data`
--

CREATE TABLE IF NOT EXISTS `counters_data_by_session_data` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `SessionByUserID` int(10) unsigned NOT NULL,
  `CounterID` int(10) unsigned NOT NULL,
  `Value` bigint(20) unsigned NOT NULL,
  PRIMARY KEY (`ID`,`CounterID`,`SessionByUserID`),
  KEY `CounterID` (`CounterID`),
  KEY `SessionByUserID` (`SessionByUserID`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=660 ;

-- --------------------------------------------------------

--
-- Дублирующая структура для представления `counters_usages_total`
--
CREATE TABLE IF NOT EXISTS `counters_usages_total` (
`Counter Name` varchar(255)
,`Uses Count` decimal(42,0)
,`Users Count` bigint(21)
);
-- --------------------------------------------------------

--
-- Структура таблицы `countries`
--

CREATE TABLE IF NOT EXISTS `countries` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `country` varchar(191) NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `country` (`country`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=194 ;

-- --------------------------------------------------------

--
-- Дублирующая структура для представления `countries_count`
--
CREATE TABLE IF NOT EXISTS `countries_count` (
`Country ID` int(10) unsigned
,`Country` varchar(191)
,`Users Count` bigint(21)
);
-- --------------------------------------------------------

--
-- Дублирующая структура для представления `last_30_days_counters_usages`
--
CREATE TABLE IF NOT EXISTS `last_30_days_counters_usages` (
`Counter Name` varchar(255)
,`Uses Count` bigint(40) unsigned
,`Users Count` bigint(21)
);
-- --------------------------------------------------------

--
-- Дублирующая структура для представления `last_30_days_program_versions_usages`
--
CREATE TABLE IF NOT EXISTS `last_30_days_program_versions_usages` (
`Version` varchar(191)
,`Program Bittness` varchar(191)
,`Users Count` bigint(21)
);
-- --------------------------------------------------------

--
-- Дублирующая структура для представления `last_30_days_sessions`
--
CREATE TABLE IF NOT EXISTS `last_30_days_sessions` (
`ID` int(10) unsigned
,`SessionName` varchar(255)
,`SessionDateTime` datetime
);
-- --------------------------------------------------------

--
-- Дублирующая структура для представления `last_30_days_unique_users`
--
CREATE TABLE IF NOT EXISTS `last_30_days_unique_users` (
`User ID` int(10) unsigned
,`User Name` varchar(191)
,`Sessions Count` bigint(21)
,`Last Sesson Date Time` datetime
);
-- --------------------------------------------------------

--
-- Дублирующая структура для представления `last_user_session`
--
CREATE TABLE IF NOT EXISTS `last_user_session` (
`User Name` varchar(191)
,`User ID` int(10) unsigned
,`Sesson Name` varchar(255)
,`Sesson ID` int(10) unsigned
,`Sesson Date Time` datetime
);
-- --------------------------------------------------------

--
-- Структура таблицы `locale_languages`
--

CREATE TABLE IF NOT EXISTS `locale_languages` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `localeLanguage` varchar(191) NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `localeLanguage` (`localeLanguage`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=194 ;

-- --------------------------------------------------------

--
-- Структура таблицы `os`
--

CREATE TABLE IF NOT EXISTS `os` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `os` varchar(191) NOT NULL,
  `bittness` varchar(191) NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `os` (`os`,`bittness`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=194 ;

-- --------------------------------------------------------

--
-- Дублирующая структура для представления `os_count`
--
CREATE TABLE IF NOT EXISTS `os_count` (
`ID` int(10) unsigned
,`OS` varchar(191)
,`bittness` varchar(191)
,`Count` bigint(21)
);
-- --------------------------------------------------------

--
-- Структура таблицы `program_versions`
--

CREATE TABLE IF NOT EXISTS `program_versions` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `version` varchar(191) NOT NULL,
  `programBittness` varchar(191) NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `version` (`version`,`programBittness`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=194 ;

-- --------------------------------------------------------

--
-- Дублирующая структура для представления `program_versions_total_usages`
--
CREATE TABLE IF NOT EXISTS `program_versions_total_usages` (
`Version` varchar(191)
,`Program Bittness` varchar(191)
,`Users Count` bigint(21)
);
-- --------------------------------------------------------

--
-- Структура таблицы `sessions`
--

CREATE TABLE IF NOT EXISTS `sessions` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `SessionName` varchar(255) NOT NULL,
  `SessionDateTime` datetime NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `SessionName_UNIQUE` (`SessionName`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=194 ;

-- --------------------------------------------------------

--
-- Структура таблицы `session_by_user`
--

CREATE TABLE IF NOT EXISTS `session_by_user` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `SessionID` int(10) unsigned NOT NULL,
  `UserID` int(10) unsigned NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `SessionID` (`SessionID`),
  KEY `UserID` (`UserID`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=194 ;

-- --------------------------------------------------------

--
-- Структура таблицы `users`
--

CREATE TABLE IF NOT EXISTS `users` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `UserName` varchar(191) NOT NULL,
  `OSID` int(10) unsigned NOT NULL,
  `VersionID` int(10) unsigned NOT NULL,
  `CountryID` int(10) unsigned NOT NULL,
  `LanguageID` int(10) unsigned NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `UserID` (`UserName`),
  KEY `VersionID` (`VersionID`),
  KEY `OSID_idx` (`OSID`),
  KEY `CountryID` (`CountryID`),
  KEY `LanguageID` (`LanguageID`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=38 ;

-- --------------------------------------------------------

--
-- Структура для представления `all_users`
--
DROP TABLE IF EXISTS `all_users`;

CREATE ALGORITHM=UNDEFINED DEFINER=`046767862_stats`@`%` SQL SECURITY DEFINER VIEW `all_users` AS select `users`.`UserName` AS `UserName`,`os`.`os` AS `OS`,`program_versions`.`version` AS `Program version`,`program_versions`.`programBittness` AS `Program bittnes`,`countries`.`country` AS `Country`,`locale_languages`.`localeLanguage` AS `Locale language` from ((((`users` join `os` on((`users`.`OSID` = `os`.`ID`))) join `program_versions` on((`users`.`VersionID` = `program_versions`.`ID`))) join `countries` on((`users`.`CountryID` = `countries`.`ID`))) join `locale_languages` on((`users`.`LanguageID` = `locale_languages`.`ID`)));

-- --------------------------------------------------------

--
-- Структура для представления `counters_usages_total`
--
DROP TABLE IF EXISTS `counters_usages_total`;

CREATE ALGORITHM=UNDEFINED DEFINER=`046767862_stats`@`%` SQL SECURITY DEFINER VIEW `counters_usages_total` AS select `counters`.`CounterName` AS `Counter Name`,sum(`counters_data_by_session_data`.`Value`) AS `Uses Count`,count(`session_by_user`.`UserID`) AS `Users Count` from ((`counters_data_by_session_data` join `counters` on((`counters_data_by_session_data`.`CounterID` = `counters`.`ID`))) join `session_by_user` on((`counters_data_by_session_data`.`SessionByUserID` = `session_by_user`.`ID`))) group by `counters_data_by_session_data`.`CounterID`;

-- --------------------------------------------------------

--
-- Структура для представления `countries_count`
--
DROP TABLE IF EXISTS `countries_count`;

CREATE ALGORITHM=UNDEFINED DEFINER=`046767862_stats`@`%` SQL SECURITY DEFINER VIEW `countries_count` AS select `countries`.`ID` AS `Country ID`,`countries`.`country` AS `Country`,count(`users`.`CountryID`) AS `Users Count` from (`users` join `countries` on((`users`.`CountryID` = `countries`.`ID`))) group by `countries`.`country`;

-- --------------------------------------------------------

--
-- Структура для представления `last_30_days_counters_usages`
--
DROP TABLE IF EXISTS `last_30_days_counters_usages`;

CREATE ALGORITHM=UNDEFINED DEFINER=`046767862_stats`@`%` SQL SECURITY DEFINER VIEW `last_30_days_counters_usages` AS select `counters`.`CounterName` AS `Counter Name`,(count(`counters_data_by_session_data`.`CounterID`) * `counters_data_by_session_data`.`Value`) AS `Uses Count`,count(`session_by_user`.`UserID`) AS `Users Count` from (((`counters_data_by_session_data` join `counters` on((`counters_data_by_session_data`.`CounterID` = `counters`.`ID`))) join `session_by_user` on((`counters_data_by_session_data`.`SessionByUserID` = `session_by_user`.`ID`))) join `sessions` on((`sessions`.`ID` = `session_by_user`.`SessionID`))) where ((`sessions`.`SessionDateTime` >= (now() - interval 30 day)) and (`sessions`.`SessionDateTime` <= now())) group by `counters_data_by_session_data`.`CounterID`;

-- --------------------------------------------------------

--
-- Структура для представления `last_30_days_program_versions_usages`
--
DROP TABLE IF EXISTS `last_30_days_program_versions_usages`;

CREATE ALGORITHM=UNDEFINED DEFINER=`046767862_stats`@`%` SQL SECURITY DEFINER VIEW `last_30_days_program_versions_usages` AS select `program_versions`.`version` AS `Version`,`program_versions`.`programBittness` AS `Program Bittness`,count(`users`.`VersionID`) AS `Users Count` from (((`program_versions` join `users` on((`program_versions`.`ID` = `users`.`VersionID`))) join `session_by_user` on((`users`.`ID` = `session_by_user`.`UserID`))) join `sessions` on((`sessions`.`ID` = `session_by_user`.`SessionID`))) where ((`sessions`.`SessionDateTime` >= (now() - interval 30 day)) and (`sessions`.`SessionDateTime` <= now())) group by `program_versions`.`ID`;

-- --------------------------------------------------------

--
-- Структура для представления `last_30_days_sessions`
--
DROP TABLE IF EXISTS `last_30_days_sessions`;

CREATE ALGORITHM=UNDEFINED DEFINER=`046767862_stats`@`%` SQL SECURITY DEFINER VIEW `last_30_days_sessions` AS select `sessions`.`ID` AS `ID`,`sessions`.`SessionName` AS `SessionName`,`sessions`.`SessionDateTime` AS `SessionDateTime` from `sessions` where ((`sessions`.`SessionDateTime` >= (now() - interval 30 day)) and (`sessions`.`SessionDateTime` <= now()));

-- --------------------------------------------------------

--
-- Структура для представления `last_30_days_unique_users`
--
DROP TABLE IF EXISTS `last_30_days_unique_users`;

CREATE ALGORITHM=UNDEFINED DEFINER=`046767862_stats`@`%` SQL SECURITY DEFINER VIEW `last_30_days_unique_users` AS select `users`.`ID` AS `User ID`,`users`.`UserName` AS `User Name`,count(`session_by_user`.`SessionID`) AS `Sessions Count`,max(`sessions`.`SessionDateTime`) AS `Last Sesson Date Time` from ((`users` join `session_by_user` on((`users`.`ID` = `session_by_user`.`UserID`))) join `sessions` on((`sessions`.`ID` = `session_by_user`.`SessionID`))) where ((`sessions`.`SessionDateTime` >= (now() - interval 30 day)) and (`sessions`.`SessionDateTime` <= now())) group by `users`.`UserName`;

-- --------------------------------------------------------

--
-- Структура для представления `last_user_session`
--
DROP TABLE IF EXISTS `last_user_session`;

CREATE ALGORITHM=UNDEFINED DEFINER=`046767862_stats`@`%` SQL SECURITY DEFINER VIEW `last_user_session` AS select `users`.`UserName` AS `User Name`,`users`.`ID` AS `User ID`,`sessions`.`SessionName` AS `Sesson Name`,`sessions`.`ID` AS `Sesson ID`,max(`sessions`.`SessionDateTime`) AS `Sesson Date Time` from ((`users` join `session_by_user` on((`users`.`ID` = `session_by_user`.`UserID`))) join `sessions` on((`sessions`.`ID` = `session_by_user`.`SessionID`))) group by `users`.`UserName`;

-- --------------------------------------------------------

--
-- Структура для представления `os_count`
--
DROP TABLE IF EXISTS `os_count`;

CREATE ALGORITHM=UNDEFINED DEFINER=`046767862_stats`@`%` SQL SECURITY DEFINER VIEW `os_count` AS select `os`.`ID` AS `ID`,`os`.`os` AS `OS`,`os`.`bittness` AS `bittness`,count(`users`.`OSID`) AS `Count` from (`users` join `os` on((`users`.`OSID` = `os`.`ID`))) group by `os`.`ID`;

-- --------------------------------------------------------

--
-- Структура для представления `program_versions_total_usages`
--
DROP TABLE IF EXISTS `program_versions_total_usages`;

CREATE ALGORITHM=UNDEFINED DEFINER=`046767862_stats`@`%` SQL SECURITY DEFINER VIEW `program_versions_total_usages` AS select `program_versions`.`version` AS `Version`,`program_versions`.`programBittness` AS `Program Bittness`,count(`users`.`VersionID`) AS `Users Count` from (`program_versions` join `users` on((`program_versions`.`ID` = `users`.`VersionID`))) group by `program_versions`.`ID`;

--
-- Ограничения внешнего ключа сохраненных таблиц
--

--
-- Ограничения внешнего ключа таблицы `counters_data_by_session_data`
--
ALTER TABLE `counters_data_by_session_data`
  ADD CONSTRAINT `CounterID` FOREIGN KEY (`CounterID`) REFERENCES `counters` (`ID`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  ADD CONSTRAINT `SessionByUserID	` FOREIGN KEY (`SessionByUserID`) REFERENCES `session_by_user` (`ID`) ON DELETE NO ACTION ON UPDATE NO ACTION;

--
-- Ограничения внешнего ключа таблицы `session_by_user`
--
ALTER TABLE `session_by_user`
  ADD CONSTRAINT `SessionID` FOREIGN KEY (`SessionID`) REFERENCES `sessions` (`ID`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  ADD CONSTRAINT `UserID` FOREIGN KEY (`UserID`) REFERENCES `users` (`ID`) ON DELETE NO ACTION ON UPDATE NO ACTION;

--
-- Ограничения внешнего ключа таблицы `users`
--
ALTER TABLE `users`
  ADD CONSTRAINT `CountryID` FOREIGN KEY (`CountryID`) REFERENCES `countries` (`ID`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  ADD CONSTRAINT `LanguageID` FOREIGN KEY (`LanguageID`) REFERENCES `locale_languages` (`ID`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  ADD CONSTRAINT `OSID` FOREIGN KEY (`OSID`) REFERENCES `os` (`ID`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  ADD CONSTRAINT `VersionID` FOREIGN KEY (`VersionID`) REFERENCES `program_versions` (`ID`) ON DELETE NO ACTION ON UPDATE NO ACTION;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
