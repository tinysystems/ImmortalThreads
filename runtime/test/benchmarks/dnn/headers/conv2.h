#ifndef CONV2_H
#define CONV2_H
#include <includes/fixedlib/fixed.h>

#define CONV2_WM_LEN 931

__ro_hifram fixed conv2_wm[931] = {
    F_LIT(0.3609372),      F_LIT(-0.13929456),   F_LIT(-0.06411328),
    F_LIT(-0.14308593),    F_LIT(-0.07196701),   F_LIT(-0.0033705407),
    F_LIT(-0.107480586),   F_LIT(-0.2735574),    F_LIT(-0.16289745),
    F_LIT(-0.22797589),    F_LIT(-0.25277436),   F_LIT(0.24958108),
    F_LIT(0.26707667),     F_LIT(0.23151354),    F_LIT(0.2172853),
    F_LIT(0.115045585),    F_LIT(-0.14800364),   F_LIT(-0.14042354),
    F_LIT(-0.095462374),   F_LIT(-0.083515786),  F_LIT(-0.13962139),
    F_LIT(-0.14522715),    F_LIT(-0.22158697),   F_LIT(-0.09876421),
    F_LIT(-0.16188627),    F_LIT(-0.17538945),   F_LIT(0.15698653),
    F_LIT(0.19303845),     F_LIT(0.21112143),    F_LIT(0.23652866),
    F_LIT(-0.1766927),     F_LIT(0.20902605),    F_LIT(0.17699932),
    F_LIT(0.15415812),     F_LIT(-0.0985282),    F_LIT(-0.042117666),
    F_LIT(-0.16297174),    F_LIT(-0.32517922),   F_LIT(-0.2753498),
    F_LIT(0.22888698),     F_LIT(0.27671197),    F_LIT(0.24031554),
    F_LIT(-0.040725734),   F_LIT(-0.05759161),   F_LIT(-0.2339043),
    F_LIT(-0.3816181),     F_LIT(-0.1709738),    F_LIT(0.0064499252),
    F_LIT(-0.06267423),    F_LIT(-0.1509559),    F_LIT(-0.09641537),
    F_LIT(0.16305862),     F_LIT(0.16267532),    F_LIT(0.114959866),
    F_LIT(0.1123951),      F_LIT(0.19046764),    F_LIT(0.24091792),
    F_LIT(-0.18175228),    F_LIT(-0.27907127),   F_LIT(0.26430374),
    F_LIT(0.25816596),     F_LIT(0.15859005),    F_LIT(-0.19444515),
    F_LIT(0.11114047),     F_LIT(0.18390478),    F_LIT(-0.13913523),
    F_LIT(-0.08807733),    F_LIT(0.21759671),    F_LIT(-0.056879543),
    F_LIT(-0.16727133),    F_LIT(0.217647),      F_LIT(0.10796722),
    F_LIT(0.14178887),     F_LIT(-0.21224348),   F_LIT(0.1366084),
    F_LIT(0.21153629),     F_LIT(-0.22805062),   F_LIT(0.179414),
    F_LIT(0.15644209),     F_LIT(-0.13518319),   F_LIT(-0.26165405),
    F_LIT(-0.29142842),    F_LIT(0.33904526),    F_LIT(0.3045872),
    F_LIT(0.14731996),     F_LIT(-0.17054336),   F_LIT(0.30300108),
    F_LIT(-0.32187614),    F_LIT(0.23766345),    F_LIT(-0.12529898),
    F_LIT(-0.18525255),    F_LIT(0.019362196),   F_LIT(0.14267227),
    F_LIT(-0.066376194),   F_LIT(-0.16312338),   F_LIT(0.08112866),
    F_LIT(-0.11497499),    F_LIT(-0.114261165),  F_LIT(-0.1689259),
    F_LIT(-0.14982295),    F_LIT(-0.19457501),   F_LIT(0.1586672),
    F_LIT(-0.15675522),    F_LIT(-0.18494356),   F_LIT(0.11020727),
    F_LIT(0.2773906),      F_LIT(-0.09160342),   F_LIT(-0.15863352),
    F_LIT(-0.08641966),    F_LIT(-0.13328733),   F_LIT(-0.100243755),
    F_LIT(-0.0980888),     F_LIT(-0.085418165),  F_LIT(-0.12302333),
    F_LIT(-0.080859534),   F_LIT(-0.08678798),   F_LIT(-0.09673444),
    F_LIT(-0.092213556),   F_LIT(-0.113264404),  F_LIT(-0.06574896),
    F_LIT(0.20803191),     F_LIT(-0.059535686),  F_LIT(-0.20031245),
    F_LIT(0.21986046),     F_LIT(-0.13716839),   F_LIT(-0.06557447),
    F_LIT(-0.09797433),    F_LIT(-0.101940125),  F_LIT(-0.12005001),
    F_LIT(0.16335763),     F_LIT(-0.20774013),   F_LIT(0.2930879),
    F_LIT(0.21451426),     F_LIT(-0.33221337),   F_LIT(0.20128575),
    F_LIT(-0.09210249),    F_LIT(-0.0828328),    F_LIT(-0.07360573),
    F_LIT(-0.055397853),   F_LIT(-0.14062525),   F_LIT(0.134806),
    F_LIT(-0.13377507),    F_LIT(-0.17868239),   F_LIT(-0.15986684),
    F_LIT(-0.14813046),    F_LIT(0.009828783),   F_LIT(-0.2903861),
    F_LIT(0.24053517),     F_LIT(0.12996808),    F_LIT(0.23213819),
    F_LIT(0.16037333),     F_LIT(0.22550586),    F_LIT(0.08038039),
    F_LIT(0.17181098),     F_LIT(0.2095191),     F_LIT(0.2994371),
    F_LIT(0.2346403),      F_LIT(-0.052045558),  F_LIT(-0.20150958),
    F_LIT(0.24690664),     F_LIT(0.31861648),    F_LIT(-0.11781082),
    F_LIT(-0.19564925),    F_LIT(-0.16128547),   F_LIT(-0.091589585),
    F_LIT(-0.10853873),    F_LIT(-0.06517664),   F_LIT(-0.12352769),
    F_LIT(-0.10310088),    F_LIT(0.21637334),    F_LIT(0.27449697),
    F_LIT(0.2590351),      F_LIT(0.19518654),    F_LIT(0.22376932),
    F_LIT(0.05735408),     F_LIT(0.00083802914), F_LIT(-0.1670655),
    F_LIT(-0.14177682),    F_LIT(-0.18917364),   F_LIT(-0.22416501),
    F_LIT(-0.08660479),    F_LIT(-0.20965192),   F_LIT(-0.14300439),
    F_LIT(-0.1180437),     F_LIT(0.090059586),   F_LIT(0.13985397),
    F_LIT(0.23499921),     F_LIT(0.25684708),    F_LIT(-0.19229473),
    F_LIT(-0.15189227),    F_LIT(0.000633089),   F_LIT(0.31979463),
    F_LIT(0.31759387),     F_LIT(-0.11876637),   F_LIT(-0.19490325),
    F_LIT(-0.210979),      F_LIT(-0.21452464),   F_LIT(-0.2132381),
    F_LIT(-0.115688555),   F_LIT(-0.11504711),   F_LIT(-0.11230408),
    F_LIT(-0.14070489),    F_LIT(0.28076756),    F_LIT(0.24226594),
    F_LIT(0.2524954),      F_LIT(-0.18778172),   F_LIT(-0.11733316),
    F_LIT(-0.21297215),    F_LIT(-0.0049772505), F_LIT(-0.065803856),
    F_LIT(-0.1837601),     F_LIT(-0.11910885),   F_LIT(-0.13883607),
    F_LIT(-0.074483305),   F_LIT(-0.1644493),    F_LIT(-0.13658844),
    F_LIT(-0.11986693),    F_LIT(-0.1007273),    F_LIT(-0.10516408),
    F_LIT(-0.117352),      F_LIT(-0.10779163),   F_LIT(-0.13680543),
    F_LIT(-0.15114696),    F_LIT(-0.15629674),   F_LIT(-0.19798023),
    F_LIT(-0.17590353),    F_LIT(-0.09754398),   F_LIT(-0.106566094),
    F_LIT(-0.18887123),    F_LIT(-0.121250436),  F_LIT(-0.19053362),
    F_LIT(0.1920918),      F_LIT(0.20366284),    F_LIT(0.09898553),
    F_LIT(0.13869797),     F_LIT(0.06750669),    F_LIT(0.16671205),
    F_LIT(-0.029207718),   F_LIT(0.17891459),    F_LIT(0.10360963),
    F_LIT(0.11715047),     F_LIT(-0.10794133),   F_LIT(0.12042143),
    F_LIT(-0.049855914),   F_LIT(-0.1646569),    F_LIT(-0.18569434),
    F_LIT(-0.16146585),    F_LIT(0.15297948),    F_LIT(0.20353837),
    F_LIT(-0.11838823),    F_LIT(0.21517058),    F_LIT(0.17876023),
    F_LIT(0.2893026),      F_LIT(-0.28953087),   F_LIT(0.1831816),
    F_LIT(-0.09238038),    F_LIT(-0.09710218),   F_LIT(-0.10888995),
    F_LIT(-0.18856001),    F_LIT(-0.04857404),   F_LIT(-0.09452506),
    F_LIT(0.22832344),     F_LIT(0.21212195),    F_LIT(0.258496),
    F_LIT(0.24995556),     F_LIT(0.1918491),     F_LIT(0.1908925),
    F_LIT(0.15071803),     F_LIT(0.124239765),   F_LIT(-0.23745088),
    F_LIT(0.12811458),     F_LIT(0.19935733),    F_LIT(-0.3270073),
    F_LIT(0.12909728),     F_LIT(0.18021038),    F_LIT(0.11352672),
    F_LIT(-0.2142028),     F_LIT(0.17392692),    F_LIT(0.022139752),
    F_LIT(-0.1077402),     F_LIT(-0.23496243),   F_LIT(-0.19659473),
    F_LIT(-0.23847356),    F_LIT(0.26912686),    F_LIT(0.2006939),
    F_LIT(-0.1211255),     F_LIT(-0.0665028),    F_LIT(0.044924006),
    F_LIT(0.000103153754), F_LIT(-0.041018095),  F_LIT(0.21924198),
    F_LIT(0.24546112),     F_LIT(0.23772173),    F_LIT(-0.100158945),
    F_LIT(-0.0075261397),  F_LIT(0.002293015),   F_LIT(0.21545161),
    F_LIT(0.002007858),    F_LIT(-0.07043856),   F_LIT(0.28850076),
    F_LIT(-0.22164354),    F_LIT(0.26678225),    F_LIT(-0.3897417),
    F_LIT(-0.32172164),    F_LIT(-0.26463434),   F_LIT(-0.22436391),
    F_LIT(-0.26250747),    F_LIT(-0.42949244),   F_LIT(0.31446487),
    F_LIT(0.21229796),     F_LIT(0.29294154),    F_LIT(0.07438679),
    F_LIT(0.012552242),    F_LIT(0.016550511),   F_LIT(0.16684845),
    F_LIT(0.05582001),     F_LIT(0.06829512),    F_LIT(0.17231885),
    F_LIT(0.095474415),    F_LIT(0.15231751),    F_LIT(-0.048278496),
    F_LIT(0.015778136),    F_LIT(-0.08306435),   F_LIT(0.15313838),
    F_LIT(0.25429973),     F_LIT(0.25311548),    F_LIT(0.1985296),
    F_LIT(0.23958313),     F_LIT(0.21723023),    F_LIT(0.19670793),
    F_LIT(0.18572447),     F_LIT(0.14518651),    F_LIT(0.17521746),
    F_LIT(-0.119564965),   F_LIT(0.34822312),    F_LIT(-0.06512244),
    F_LIT(0.039122093),    F_LIT(-0.1095716),    F_LIT(-0.06285296),
    F_LIT(-0.15004161),    F_LIT(-0.12685855),   F_LIT(-0.1003501),
    F_LIT(-0.19628981),    F_LIT(-0.10276548),   F_LIT(-0.17231107),
    F_LIT(0.25308597),     F_LIT(-0.17054379),   F_LIT(-0.15636091),
    F_LIT(-0.04803731),    F_LIT(0.18901284),    F_LIT(0.24176955),
    F_LIT(0.23903608),     F_LIT(0.072959274),   F_LIT(0.35712868),
    F_LIT(0.24387857),     F_LIT(-0.108851865),  F_LIT(0.261908),
    F_LIT(0.26616204),     F_LIT(0.21004906),    F_LIT(0.15750854),
    F_LIT(0.057480298),    F_LIT(0.16466837),    F_LIT(0.092472196),
    F_LIT(-0.19601434),    F_LIT(-0.042317946),  F_LIT(-0.021690736),
    F_LIT(0.008183056),    F_LIT(-0.0528704),    F_LIT(-0.071703285),
    F_LIT(-0.10727592),    F_LIT(-0.048622824),  F_LIT(-0.12070469),
    F_LIT(-0.1965082),     F_LIT(-0.13961099),   F_LIT(-0.16628413),
    F_LIT(0.17848666),     F_LIT(0.16176373),    F_LIT(0.18479626),
    F_LIT(0.1539367),      F_LIT(-0.14538375),   F_LIT(-0.2987538),
    F_LIT(-0.3339671),     F_LIT(-0.18503433),   F_LIT(-0.117555775),
    F_LIT(0.08165695),     F_LIT(-0.30004737),   F_LIT(0.01375495),
    F_LIT(0.07371402),     F_LIT(0.10919031),    F_LIT(0.11943796),
    F_LIT(0.034280583),    F_LIT(0.26621863),    F_LIT(0.0973491),
    F_LIT(-0.04236187),    F_LIT(0.15143847),    F_LIT(0.10789103),
    F_LIT(0.1295023),      F_LIT(-0.30876866),   F_LIT(-0.01687879),
    F_LIT(-0.3087047),     F_LIT(-0.08950986),   F_LIT(0.20117103),
    F_LIT(-0.058297683),   F_LIT(-0.12805395),   F_LIT(-0.030029206),
    F_LIT(-0.1263784),     F_LIT(-0.24088499),   F_LIT(-0.10628603),
    F_LIT(-0.3526143),     F_LIT(-0.13513698),   F_LIT(-0.33552366),
    F_LIT(0.042460997),    F_LIT(0.26322916),    F_LIT(0.25610715),
    F_LIT(-0.24908142),    F_LIT(0.17312008),    F_LIT(0.22877158),
    F_LIT(0.2809726),      F_LIT(-0.24325593),   F_LIT(-0.2536631),
    F_LIT(0.057123985),    F_LIT(0.1402639),     F_LIT(0.21278456),
    F_LIT(-0.102013044),   F_LIT(-0.16778366),   F_LIT(-0.07225509),
    F_LIT(-0.083316326),   F_LIT(-0.19162944),   F_LIT(0.37322176),
    F_LIT(0.35627857),     F_LIT(0.39580697),    F_LIT(0.36475915),
    F_LIT(-0.1330928),     F_LIT(-0.13518465),   F_LIT(-0.098797224),
    F_LIT(-0.15205795),    F_LIT(-0.16772737),   F_LIT(-0.18210779),
    F_LIT(-0.17219463),    F_LIT(-0.07145281),   F_LIT(-0.010535231),
    F_LIT(-0.010235855),   F_LIT(0.005632981),   F_LIT(0.21222138),
    F_LIT(0.24464747),     F_LIT(0.26817203),    F_LIT(-0.14133011),
    F_LIT(-0.1130225),     F_LIT(0.22537626),    F_LIT(-0.24173637),
    F_LIT(-0.097768776),   F_LIT(-0.25162718),   F_LIT(-0.2460939),
    F_LIT(-0.0031260476),  F_LIT(-0.09868135),   F_LIT(0.059743736),
    F_LIT(-0.11924223),    F_LIT(-0.18599999),   F_LIT(-0.25308827),
    F_LIT(-0.08022718),    F_LIT(-0.080544844),  F_LIT(-0.17700517),
    F_LIT(0.23586205),     F_LIT(-0.071702324),  F_LIT(0.25683475),
    F_LIT(0.24758649),     F_LIT(0.21687962),    F_LIT(-0.24250644),
    F_LIT(-0.05323036),    F_LIT(-0.13965562),   F_LIT(-0.036013793),
    F_LIT(0.018389072),    F_LIT(0.22290613),    F_LIT(0.13545637),
    F_LIT(0.2077471),      F_LIT(0.13773757),    F_LIT(0.1856966),
    F_LIT(-0.034103185),   F_LIT(-0.029724386),  F_LIT(0.27857235),
    F_LIT(0.13772146),     F_LIT(-0.30659184),   F_LIT(0.31284314),
    F_LIT(0.30074814),     F_LIT(0.25728655),    F_LIT(0.2852359),
    F_LIT(0.036419276),    F_LIT(-0.10134807),   F_LIT(-0.1412919),
    F_LIT(-0.24192289),    F_LIT(-0.23383264),   F_LIT(-0.2820977),
    F_LIT(-0.007006676),   F_LIT(-0.012090355),  F_LIT(0.31705174),
    F_LIT(-0.3322192),     F_LIT(-0.30774918),   F_LIT(-0.16728625),
    F_LIT(-0.11213621),    F_LIT(-0.12555277),   F_LIT(0.20272276),
    F_LIT(0.21773617),     F_LIT(-0.08194873),   F_LIT(0.0065191253),
    F_LIT(-0.10664795),    F_LIT(0.2696068),     F_LIT(-0.25125885),
    F_LIT(-0.14023472),    F_LIT(0.2695609),     F_LIT(0.30614233),
    F_LIT(-0.03421077),    F_LIT(-0.11942889),   F_LIT(0.13550083),
    F_LIT(-0.3687053),     F_LIT(-0.21482548),   F_LIT(-0.19958164),
    F_LIT(-0.119764194),   F_LIT(-0.07946061),   F_LIT(-0.15701167),
    F_LIT(-0.24402474),    F_LIT(-0.092683576),  F_LIT(-0.049001426),
    F_LIT(-0.11840297),    F_LIT(0.2847794),     F_LIT(-0.04195437),
    F_LIT(0.2825308),      F_LIT(0.15043299),    F_LIT(-0.101299085),
    F_LIT(0.18600558),     F_LIT(-0.13361087),   F_LIT(0.25408953),
    F_LIT(-0.022013877),   F_LIT(-0.100756176),  F_LIT(0.20273148),
    F_LIT(0.23114823),     F_LIT(0.28480932),    F_LIT(-0.080262855),
    F_LIT(-0.061364714),   F_LIT(0.19596325),    F_LIT(0.23723282),
    F_LIT(0.12598678),     F_LIT(0.13975687),    F_LIT(0.17410089),
    F_LIT(0.25548363),     F_LIT(0.2931972),     F_LIT(0.2142407),
    F_LIT(0.26100168),     F_LIT(0.23314238),    F_LIT(-0.23792288),
    F_LIT(-0.3040619),     F_LIT(-0.23295285),   F_LIT(0.03250874),
    F_LIT(-0.0719725),     F_LIT(-0.12291464),   F_LIT(-0.06003021),
    F_LIT(0.10907556),     F_LIT(0.23167075),    F_LIT(0.2694332),
    F_LIT(-0.061701704),   F_LIT(0.037059773),   F_LIT(-0.24010853),
    F_LIT(-0.038000412),   F_LIT(-0.075293586),  F_LIT(-0.093292534),
    F_LIT(0.022806913),    F_LIT(-0.25196403),   F_LIT(0.14282925),
    F_LIT(0.08759279),     F_LIT(0.2994996),     F_LIT(0.33892456),
    F_LIT(-0.21341994),    F_LIT(-0.10207071),   F_LIT(-0.20651239),
    F_LIT(-0.040548764),   F_LIT(-0.0049930788), F_LIT(0.2979177),
    F_LIT(-0.12088557),    F_LIT(0.02054977),    F_LIT(-0.06818683),
    F_LIT(-0.12445984),    F_LIT(-0.24935207),   F_LIT(-0.25726202),
    F_LIT(0.19817065),     F_LIT(0.19295435),    F_LIT(0.24076745),
    F_LIT(0.19972084),     F_LIT(0.19616354),    F_LIT(0.18994433),
    F_LIT(-0.24158716),    F_LIT(-0.16524757),   F_LIT(0.12510557),
    F_LIT(0.24498798),     F_LIT(-0.07442344),   F_LIT(-0.045469042),
    F_LIT(0.010690128),    F_LIT(-0.051708017),  F_LIT(0.17378396),
    F_LIT(0.17641677),     F_LIT(-0.12965196),   F_LIT(0.18274015),
    F_LIT(0.20693778),     F_LIT(0.0201887),     F_LIT(0.15039164),
    F_LIT(-0.2528756),     F_LIT(0.18912917),    F_LIT(0.19213837),
    F_LIT(0.27148274),     F_LIT(-0.36620387),   F_LIT(-0.27360696),
    F_LIT(-0.17346369),    F_LIT(-0.3612972),    F_LIT(-0.111024305),
    F_LIT(-0.21399303),    F_LIT(-0.07646594),   F_LIT(-0.10434217),
    F_LIT(-0.16321403),    F_LIT(-0.13653542),   F_LIT(-0.037983634),
    F_LIT(0.23323792),     F_LIT(-0.12158854),   F_LIT(-0.25841933),
    F_LIT(-0.17788702),    F_LIT(-0.19192109),   F_LIT(-0.18160161),
    F_LIT(-0.0944385),     F_LIT(-0.24604245),   F_LIT(0.18940926),
    F_LIT(-0.23565127),    F_LIT(0.19218369),    F_LIT(0.26911122),
    F_LIT(-0.23422767),    F_LIT(0.22350441),    F_LIT(-0.058448702),
    F_LIT(-0.008082486),   F_LIT(-0.16631189),   F_LIT(0.2047339),
    F_LIT(0.34688002),     F_LIT(0.31009105),    F_LIT(-0.027986556),
    F_LIT(-0.11812682),    F_LIT(0.33631453),    F_LIT(-0.024630144),
    F_LIT(-0.071995296),   F_LIT(-0.118665114),  F_LIT(-0.10524024),
    F_LIT(-0.19507155),    F_LIT(0.21200584),    F_LIT(-0.19739373),
    F_LIT(-0.32119074),    F_LIT(0.16469471),    F_LIT(-0.112004936),
    F_LIT(0.036235448),    F_LIT(0.18133786),    F_LIT(0.21182477),
    F_LIT(-0.1185598),     F_LIT(0.16556755),    F_LIT(0.16171125),
    F_LIT(-0.07051824),    F_LIT(0.2175852),     F_LIT(0.16079265),
    F_LIT(-0.3137674),     F_LIT(0.1390646),     F_LIT(-0.4210839),
    F_LIT(0.07542157),     F_LIT(-0.18684016),   F_LIT(0.200549),
    F_LIT(-0.060315154),   F_LIT(-0.18667081),   F_LIT(0.07670053),
    F_LIT(-0.00081276684), F_LIT(-0.18634902),   F_LIT(0.26419967),
    F_LIT(-0.2896311),     F_LIT(0.2366617),     F_LIT(0.3882595),
    F_LIT(0.23376329),     F_LIT(-0.09267196),   F_LIT(-0.21139464),
    F_LIT(-0.026746351),   F_LIT(-0.25614572),   F_LIT(-0.132871),
    F_LIT(-0.14489676),    F_LIT(0.18854325),    F_LIT(-0.15894662),
    F_LIT(0.22607598),     F_LIT(-0.07922863),   F_LIT(0.2205214),
    F_LIT(-0.20254901),    F_LIT(-0.13630731),   F_LIT(-0.0075893733),
    F_LIT(-0.25806764),    F_LIT(-0.06238157),   F_LIT(-0.11021933),
    F_LIT(-0.1107669),     F_LIT(0.23891957),    F_LIT(0.26219478),
    F_LIT(0.21083407),     F_LIT(0.26969692),    F_LIT(0.23281711),
    F_LIT(0.3229297),      F_LIT(-0.13333432),   F_LIT(0.2643428),
    F_LIT(-0.23222043),    F_LIT(-0.22729436),   F_LIT(0.19752157),
    F_LIT(-0.31652418),    F_LIT(-0.36793813),   F_LIT(-0.1252839),
    F_LIT(-0.12043723),    F_LIT(-0.100551024),  F_LIT(0.20005117),
    F_LIT(0.15447734),     F_LIT(-0.093151696),  F_LIT(0.25746572),
    F_LIT(-0.15151447),    F_LIT(-0.30041167),   F_LIT(-0.17373021),
    F_LIT(-0.15862982),    F_LIT(-0.33855543),   F_LIT(0.004024961),
    F_LIT(-0.3165777),     F_LIT(-0.15337688),   F_LIT(-0.034926046),
    F_LIT(0.22555067),     F_LIT(0.1981758),     F_LIT(0.20457922),
    F_LIT(0.08988285),     F_LIT(0.17049871),    F_LIT(0.081163734),
    F_LIT(0.35597888),     F_LIT(0.14730768),    F_LIT(-0.09064257),
    F_LIT(0.10712228),     F_LIT(0.2083279),     F_LIT(-0.22633646),
    F_LIT(0.0023257246),   F_LIT(-0.18742968),   F_LIT(-0.15074928),
    F_LIT(0.31791496),     F_LIT(-0.10327911),   F_LIT(0.34420076),
    F_LIT(-0.07917052),    F_LIT(-0.07608422),   F_LIT(-0.1492675),
    F_LIT(-0.17018616),    F_LIT(-0.26031053),   F_LIT(-0.12957229),
    F_LIT(0.022016535),    F_LIT(-0.24978425),   F_LIT(-0.22982271),
    F_LIT(0.21509929),     F_LIT(0.23130691),    F_LIT(0.18776688),
    F_LIT(0.16042754),     F_LIT(0.2102719),     F_LIT(0.23791605),
    F_LIT(0.25508478),     F_LIT(0.044309266),   F_LIT(-0.124940835),
    F_LIT(-0.13080841),    F_LIT(-0.15241696),   F_LIT(-0.23753451),
    F_LIT(-0.24052778),    F_LIT(-0.2122956),    F_LIT(-0.23136254),
    F_LIT(0.004051651),    F_LIT(0.11379551),    F_LIT(0.1239495),
    F_LIT(-0.20016001),    F_LIT(-0.091062814),  F_LIT(-0.027277593),
    F_LIT(0.07186187),     F_LIT(-0.058974188),  F_LIT(0.13526453),
    F_LIT(0.121295206),    F_LIT(0.18106128),    F_LIT(0.18962412),
    F_LIT(0.12579072),     F_LIT(-0.19548889),   F_LIT(0.12429857),
    F_LIT(-0.1187803),     F_LIT(-0.04153825),   F_LIT(0.16946931),
    F_LIT(0.23955876),     F_LIT(-0.11878852),   F_LIT(-0.14346664),
    F_LIT(-0.05196101),    F_LIT(-0.16392718),   F_LIT(-0.081342265),
    F_LIT(-0.15707693),    F_LIT(-0.18620162),   F_LIT(0.27152172),
    F_LIT(0.24660654),     F_LIT(-0.27671915),   F_LIT(-0.07395512),
    F_LIT(0.046735488),    F_LIT(-0.009542682),  F_LIT(-0.20726793),
    F_LIT(-0.067029245),   F_LIT(-0.12478929),   F_LIT(-0.021819223),
    F_LIT(-0.109480046),   F_LIT(-0.11219791),   F_LIT(-0.110515915),
    F_LIT(-0.11156705),    F_LIT(-0.10389635),   F_LIT(-0.1220331),
    F_LIT(-0.10932424),    F_LIT(-0.103579484),  F_LIT(-0.20694546),
    F_LIT(-0.10832633),    F_LIT(-0.16721842),   F_LIT(-0.102789566),
    F_LIT(-0.21531814),    F_LIT(0.18707773),    F_LIT(0.20451197),
    F_LIT(0.09619575),     F_LIT(0.13717863),    F_LIT(0.30072948),
    F_LIT(0.0949544),      F_LIT(0.114806525),   F_LIT(-0.16195892),
    F_LIT(0.31939697),     F_LIT(-0.093441084),  F_LIT(0.32717213),
    F_LIT(-0.05402712),    F_LIT(-0.13526337),   F_LIT(-0.097628266),
    F_LIT(-0.08673083),    F_LIT(-0.20274498),   F_LIT(-0.26819104),
    F_LIT(-0.25022534),    F_LIT(-0.043445986),  F_LIT(-0.1075437),
    F_LIT(-0.16993901),    F_LIT(0.15759678),    F_LIT(-0.15245098),
    F_LIT(0.29145312),     F_LIT(0.22987896),    F_LIT(-0.34557506),
    F_LIT(0.23433234),     F_LIT(-0.14127676),   F_LIT(-0.08125515),
    F_LIT(-0.03993988),    F_LIT(-0.12008733),   F_LIT(-0.13470785),
    F_LIT(-0.19436777),    F_LIT(-0.16946839),   F_LIT(-0.0070354035),
    F_LIT(0.12013766),     F_LIT(-0.14075753),   F_LIT(0.0026784248),
    F_LIT(0.39758214),     F_LIT(-0.1608587),    F_LIT(-0.13184227),
    F_LIT(-0.149637),      F_LIT(-0.1725033),    F_LIT(-0.111354314),
    F_LIT(-0.07780752),    F_LIT(-0.045371734),  F_LIT(-0.27329108),
    F_LIT(0.12089162),     F_LIT(0.19847897),    F_LIT(-0.23283829),
    F_LIT(0.30545264),     F_LIT(-0.034061242),  F_LIT(-0.1277116),
    F_LIT(-0.11161997),    F_LIT(-0.011874343),  F_LIT(0.26404062),
    F_LIT(-0.13043737),    F_LIT(-0.124179505),  F_LIT(-0.07547392),
    F_LIT(0.27170002),     F_LIT(-0.0018186626), F_LIT(-0.118723),
    F_LIT(-0.072386146),   F_LIT(-0.10749106),   F_LIT(-0.17378432),
    F_LIT(-0.12027356),    F_LIT(0.18812732),    F_LIT(-0.18742359),
    F_LIT(-0.13658154),    F_LIT(0.25499582),    F_LIT(0.26032612),
    F_LIT(0.21415582),     F_LIT(0.16448966),    F_LIT(-0.108300775),
    F_LIT(-0.00415785),    F_LIT(-0.10951461),   F_LIT(-0.17802893),
    F_LIT(-0.14228874),    F_LIT(-0.12856196),   F_LIT(-0.21432708),
    F_LIT(0.19758496),     F_LIT(-0.062034234),  F_LIT(0.022464985),
    F_LIT(-0.053630155),   F_LIT(-0.24587859),   F_LIT(-0.1392108),
    F_LIT(-0.1100247),     F_LIT(0.04310842),    F_LIT(-0.18370023),
    F_LIT(-0.11749912),    F_LIT(-0.10281257),   F_LIT(0.20360118),
    F_LIT(0.2397297),      F_LIT(0.2542895),     F_LIT(0.24459587),
    F_LIT(-0.005063416),   F_LIT(-0.07046577),   F_LIT(0.19284323),
    F_LIT(0.1704036),      F_LIT(0.21844372),    F_LIT(-0.01450775),
    F_LIT(0.2752233),      F_LIT(0.21644926),    F_LIT(0.17945303),
    F_LIT(0.18265693),     F_LIT(-0.041132342),  F_LIT(0.01798889),
    F_LIT(-0.124736175),   F_LIT(-0.20694187),   F_LIT(-0.18749066),
    F_LIT(-0.17434916),    F_LIT(0.042662524),   F_LIT(-0.11916516),
    F_LIT(-0.04046091),    F_LIT(-0.15802337),   F_LIT(0.19168457),
    F_LIT(0.19619259)};

__ro_hifram fixed conv2_wm_offsets[931] = {
    120, 42,  45,  5,   101, 75,  95,  4,   17, 11,  1,   7,   1,   1,   1,
    61,  4,   2,   3,   1,   8,   199, 27,  5,  5,   1,   69,  1,   4,   1,
    9,   84,  5,   5,   2,   5,   1,   216, 5,  41,  1,   1,   68,  1,   4,
    1,   91,  5,   6,   12,  1,   14,  1,   1,  3,   1,   1,   2,   1,   2,
    1,   1,   2,   2,   1,   10,  1,   2,   3,  1,   2,   12,  1,   3,   2,
    1,   3,   3,   6,   5,   9,   5,   7,   1,  1,   8,   3,   3,   3,   14,
    1,   1,   2,   2,   1,   2,   23,  1,   12, 5,   1,   3,   2,   1,   2,
    7,   11,  6,   18,  1,   24,  1,   5,   1,  19,  5,   1,   19,  24,  1,
    2,   3,   1,   2,   22,  1,   18,  6,   1,  16,  3,   3,   1,   2,   3,
    15,  1,   5,   20,  39,  65,  96,  109, 7,  22,  5,   3,   67,  2,   3,
    4,   1,   3,   1,   4,   36,  4,   2,   41, 2,   1,   2,   279, 8,   23,
    1,   4,   1,   50,  14,  4,   1,   5,   97, 2,   5,   168, 74,  25,  28,
    1,   4,   5,   6,   59,  5,   5,   5,   27, 25,  4,   43,  5,   2,   96,
    125, 25,  25,  174, 45,  4,   1,   104, 4,  1,   1,   5,   4,   3,   1,
    1,   90,  5,   178, 95,  106, 1,   52,  45, 5,   5,   45,  1,   1,   8,
    4,   14,  10,  5,   45,  5,   15,  1,   2,  2,   1,   2,   4,   1,   4,
    32,  6,   44,  6,   101, 71,  4,   111, 1,  2,   3,   3,   85,  7,   3,
    27,  5,   5,   1,   44,  5,   14,  1,   4,  1,   3,   2,   47,  1,   2,
    42,  1,   2,   1,   1,   1,   2,   1,   2,  2,   91,  75,  95,  31,  7,
    3,   70,  103, 1,   5,   42,  1,   1,   3,  64,  1,   49,  61,  8,   102,
    3,   2,   105, 275, 27,  5,   5,   1,   70, 4,   1,   5,   36,  5,   2,
    43,  5,   2,   3,   2,   31,  1,   5,   69, 1,   1,   3,   1,   3,   1,
    4,   89,  4,   16,  22,  2,   1,   49,  1,  100, 199, 1,   95,  41,  1,
    66,  53,  50,  32,  76,  4,   1,   2,   1,  1,   2,   1,   1,   35,  5,
    3,   47,  3,   278, 31,  1,   68,  5,   5,  1,   5,   336, 25,  27,  1,
    6,   3,   1,   1,   69,  5,   5,   7,   26, 1,   3,   1,   6,   62,  4,
    1,   3,   1,   2,   1,   2,   2,   33,  9,  41,  9,   2,   8,   158, 45,
    67,  32,  5,   1,   6,   14,  46,  9,   5,  15,  21,  50,  12,  67,  125,
    100, 8,   5,   8,   9,   25,  4,   1,   54, 1,   4,   1,   201, 4,   25,
    129, 16,  5,   4,   28,  1,   4,   5,   70, 5,   4,   7,   35,  52,  2,
    11,  265, 4,   34,  4,   1,   61,  1,   1,  1,   2,   3,   1,   1,   4,
    5,   3,   88,  7,   22,  4,   1,   75,  4,  1,   3,   5,   88,  14,  17,
    12,  10,  3,   1,   1,   4,   63,  4,   1,  1,   1,   4,   96,  1,   218,
    58,  1,   1,   111, 28,  15,  1,   62,  6,  4,   23,  70,  15,  15,  1,
    9,   60,  111, 133, 50,  91,  29,  5,   5,  62,  1,   3,   1,   3,   1,
    4,   4,   2,   2,   88,  2,   12,  275, 43, 1,   59,  32,  5,   10,  1,
    53,  1,   4,   1,   1,   3,   1,   1,   8,  1,   1,   80,  21,  101, 5,
    7,   1,   41,  70,  1,   4,   6,   5,   17, 5,   5,   65,  4,   1,   5,
    8,   1,   18,  23,  11,  37,  2,   5,   1,  5,   96,  75,  12,  1,   24,
    1,   1,   3,   6,   1,   52,  1,   66,  45, 1,   4,   95,  75,  80,  15,
    1,   15,  122, 7,   6,   1,   3,   20,  71, 3,   1,   1,   3,   8,   44,
    1,   4,   1,   34,  6,   4,   1,   3,   1,  1,   83,  6,   69,  6,   4,
    64,  6,   7,   5,   7,   3,   3,   1,   6,  115, 47,  127, 50,  121, 101,
    12,  107, 193, 174, 44,  66,  33,  70,  5,  2,   2,   1,   2,   2,   5,
    41,  48,  2,   3,   6,   6,   17,  3,   1,  67,  7,   2,   2,   2,   4,
    4,   2,   1,   27,  62,  11,  82,  6,   69, 6,   110, 27,  2,   4,   1,
    3,   63,  1,   1,   7,   4,   1,   3,   1,  4,   2,   38,  6,   151, 168,
    2,   4,   27,  1,   11,  39,  5,   5,   54, 2,   5,   22,  3,   1,   1,
    3,   1,   43,  20,  1,   4,   1,   3,   1,  3,   1,   1,   4,   2,   29,
    13,  37,  4,   3,   2,   2,   2,   10,  3,  75,  9,   66,  50,  29,  25,
    13,  23,  2,   1,   3,   1,   1,   64,  1,  3,   1,   1,   4,   1,   4,
    91,  219, 5,   29,  4,   77,  3,   2,   2,  1,   4,   90,  4,   2,   3,
    105, 1,   7,   1,   173, 1,   32,  13,  3,  1,   1,   37,  6,   3,   60,
    1,   4,   1,   47,  50,  5,   112, 60,  1,  45,  1,   3,   1,   1,   491,
    7,   26,  1,   4,   63,  5,   1,   4,   3,  6,   2,   34,  48,  2,   2,
    16,  83,  75,  45,  29,  25,  29,  5,   5,  5,   65,  1,   4,   5,   96,
    3,   13,  1,   25,  2,   1,   20,  29,  28, 2,   66,  21,  13,  16,  25,
    25,  26,  75,  1,   73,  103, 11,  5,   42, 57,  13,  30,  20,  99,  60,
    16,  25,  50,  41,  69,  1,   102, 1,   4,  250, 12,  6,   10,  8,   1,
    1,   62,  10,  9,   74,  124, 1,   75,  95, 113, 111, 5,   5,   17,  1,
    4,   1,   4,   1,   50,  19,  1,   4,   1,  6,   5,   37,  5,   40,  3,
    1,   1,   5,   5,   2,   82,  130, 5,   5,  43,  100, 112, 34,  5,   70,
    5};

__ro_hifram fixed conv2_wm_sizes[100] = {
    8, 14, 17, 10, 90, 4,  0, 20, 16, 19, 4,  0,  13, 6,  0,  3, 0,
    0, 0,  23, 0,  8,  28, 0, 4,  3,  6,  3,  5,  16, 13, 10, 5, 16,
    0, 9,  9,  0,  24, 14, 0, 3,  16, 14, 0,  15, 11, 19, 1,  8, 0,
    9, 0,  17, 3,  17, 4,  4, 21, 20, 1,  0,  36, 0,  1,  3,  2, 4,
    2, 13, 22, 21, 3,  0,  0, 4,  0,  36, 16, 2,  0,  12, 4,  7, 10,
    8, 0,  1,  21, 10, 19, 5, 8,  7,  14, 2,  29, 0,  2,  4};

__ro_hifram fixed conv2_b[100] = {
    F_LIT(0.196523740888),     F_LIT(0.0270340051502),
    F_LIT(-0.0171628054231),   F_LIT(0.0640425086021),
    F_LIT(0.103266447783),     F_LIT(0.151402235031),
    F_LIT(-0.0226933471859),   F_LIT(-0.0038078201469),
    F_LIT(0.0224395468831),    F_LIT(0.10781507194),
    F_LIT(0.0633652433753),    F_LIT(-0.011569284834),
    F_LIT(0.13311457634),      F_LIT(0.003920705989),
    F_LIT(-0.0061055268161),   F_LIT(0.0913253948092),
    F_LIT(-0.00120702187996),  F_LIT(-0.0184175614268),
    F_LIT(-0.0103748915717),   F_LIT(0.0579093173146),
    F_LIT(0.00916726607829),   F_LIT(0.0040731979534),
    F_LIT(0.0131594901904),    F_LIT(-0.0174309816211),
    F_LIT(0.0516680218279),    F_LIT(-0.0123871369287),
    F_LIT(0.0480610579252),    F_LIT(0.0299753230065),
    F_LIT(0.127385020256),     F_LIT(-0.000249430275289),
    F_LIT(-0.00449963985011),  F_LIT(0.15353423357),
    F_LIT(0.150118470192),     F_LIT(-0.0266412887722),
    F_LIT(-0.014088015072),    F_LIT(0.181392028928),
    F_LIT(0.0168920364231),    F_LIT(-0.0185732506216),
    F_LIT(0.0298650357872),    F_LIT(0.130665183067),
    F_LIT(0.0365493074059),    F_LIT(0.0110251847655),
    F_LIT(0.199030205607),     F_LIT(0.0383041799068),
    F_LIT(-0.0132253859192),   F_LIT(0.011047734879),
    F_LIT(0.0210250187665),    F_LIT(0.0942302197218),
    F_LIT(-0.00253081764095),  F_LIT(0.0314549580216),
    F_LIT(-0.017454713583),    F_LIT(0.141579091549),
    F_LIT(-0.00191018707119),  F_LIT(0.0290157105774),
    F_LIT(0.0390226431191),    F_LIT(0.0300275366753),
    F_LIT(-0.0290858503431),   F_LIT(0.0369864329696),
    F_LIT(0.0528944991529),    F_LIT(0.0739212036133),
    F_LIT(0.00975369755179),   F_LIT(-0.0218778699636),
    F_LIT(0.0722374841571),    F_LIT(-0.0139426458627),
    F_LIT(-0.00192885939032),  F_LIT(0.0385927893221),
    F_LIT(-0.00646644365042),  F_LIT(0.151078909636),
    F_LIT(0.0306956898421),    F_LIT(-0.0549622029066),
    F_LIT(0.0273936204612),    F_LIT(-0.0165219344199),
    F_LIT(-0.0146550824866),   F_LIT(-0.0169187374413),
    F_LIT(-0.00944347400218),  F_LIT(0.0531173124909),
    F_LIT(-0.0112824384123),   F_LIT(0.118561193347),
    F_LIT(0.0831662267447),    F_LIT(0.0743100419641),
    F_LIT(-0.00194427464157),  F_LIT(-0.0241687279195),
    F_LIT(-0.000622548046522), F_LIT(0.131776228547),
    F_LIT(0.0394919700921),    F_LIT(-0.00454293983057),
    F_LIT(-0.0109727596864),   F_LIT(0.0896088182926),
    F_LIT(0.0740780234337),    F_LIT(-0.0136159881949),
    F_LIT(0.192574903369),     F_LIT(-0.0199599247426),
    F_LIT(0.128755152225),     F_LIT(0.106993891299),
    F_LIT(0.0709483474493),    F_LIT(0.0319215543568),
    F_LIT(0.00435964530334),   F_LIT(-0.0221845600754),
    F_LIT(0.0488187856972),    F_LIT(0.0283885765821)};

#endif
