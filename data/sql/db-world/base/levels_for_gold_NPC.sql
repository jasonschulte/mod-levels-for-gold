SET
@Entry = 172234,
@Name = "Treets";

REPLACE INTO `creature_template` (`entry`, `modelid1`, `modelid2`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(@Entry, 1455, 0, @Name, 'Levels for Gold', NULL, 0, 6, 6, 0, 35, 1, 0.5, 0, 0, 2000, 0, 1, 0, 10, 134217728, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 2, 'npc_levels_for_gold');