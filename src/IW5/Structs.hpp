// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

namespace ZoneTool
{
	namespace IW5
	{
		enum XAssetType : std::int32_t
		{
			physpreset,
			phys_collmap,
			xanim,
			xmodelsurfs,
			xmodel,
			material,
			pixelshader,
			vertexshader,
			vertexdecl,
			techset,
			image,
			sound,
			sndcurve,
			loaded_sound,
			col_map_mp,
			com_map,
			glass_map,
			ai_paths,
			vehicle_tracks,
			map_ents,
			fx_map,
			gfx_map,
			lightdef,
			ui_map,
			// not used
			font,
			menufile,
			menu,
			localize,
			attachment,
			weapon,
			snddriverglobals,
			// not used
			fx,
			impactfx,
			surfacefx,
			aitype,
			// not used
			mptype,
			// not used
			character,
			// not used
			xmodelalias,
			// not used
			rawfile,
			scriptfile,
			stringtable,
			leaderboarddef,
			structureddatadef,
			tracer,
			vehicle,
			addon_map_ents,
			max,

			col_map_sp = col_map_mp,
		};

		typedef float vec4_t[4];
		typedef float vec3_t[3];
		typedef float vec2_t[2];

		template <std::size_t N>
		struct VecInternal
		{
			float data[N];
		};

		// Localized Strings
		struct LocalizeEntry
		{
			const char* localizedString;
			const char* name;
		};

		// Stringtables
		struct StringTableCell
		{
			char* string; // 0
			int hash; // 4
		};

		struct StringTable
		{
			const char* name; // 0
			int columns; // 4
			int rows; // 8
			StringTableCell* strings; // 12
		};

		// RawFile
		struct RawFile
		{
			const char* name;
			int compressedLen;
			int len;
			const char* buffer;
		};

		// Menus
		struct MenuEventHandlerSet;
		struct Statement_s;
		struct menuDef_t;
		struct Material;
		struct snd_alias_list_t;

		enum EventType
		{
			EVENT_UNCONDITIONAL = 0x0,
			EVENT_IF = 0x1,
			EVENT_ELSE = 0x2,
			EVENT_SET_LOCAL_VAR_BOOL = 0x3,
			EVENT_SET_LOCAL_VAR_INT = 0x4,
			EVENT_SET_LOCAL_VAR_FLOAT = 0x5,
			EVENT_SET_LOCAL_VAR_STRING = 0x6,
			EVENT_COUNT = 0x7,
		};

		enum operationEnum
		{
			OP_NOOP,
			OP_RIGHTPAREN,
			OP_MULTIPLY,
			OP_DIVIDE,
			OP_MODULUS,
			OP_ADD,
			OP_SUBTRACT,
			OP_NOT,
			OP_LESSTHAN,
			OP_LESSTHANEQUALTO,
			OP_GREATERTHAN,
			OP_GREATERTHANEQUALTO,
			OP_EQUALS,
			OP_NOTEQUAL,
			OP_AND,
			OP_OR,
			OP_LEFTPAREN,
			OP_COMMA,
			OP_BITWISEAND,
			OP_BITWISEOR,
			OP_BITWISENOT,
			OP_BITSHIFTLEFT,
			OP_BITSHIFTRIGHT,
			OP_FIRSTFUNCTIONCALL = 0x17,
			OP_STATICDVARINT = 0x17,
			OP_STATICDVARBOOL,
			OP_STATICDVARFLOAT,
			OP_STATICDVARSTRING,
			OP_INT,
			OP_STRING,
			OP_FLOAT,
			OP_SIN,
			OP_COS,
			OP_MIN,
			OP_MAX,
			OP_MILLISECONDS,
			OP_LOCALCLIENTUIMILLISECONDS,
			OP_DVARINT,
			OP_DVARBOOL,
			OP_DVARFLOAT,
			OP_DVARSTRING,
			OP_UI_ACTIVE,
			OP_FLASHBANGED,
			OP_USINGVEHICLE,
			OP_MISSILECAM,
			OP_SCOPED,
			OP_SCOPEDTHERMAL,
			OP_SCOREBOARD_VISIBLE,
			OP_INKILLCAM,
			OP_INKILLCAMNPC,
			OP_PLAYER,
			OP_GETPERK,
			OP_SELECTING_LOCATION,
			OP_SELECTING_DIRECTION,
			OP_TEAM,
			OP_OTHERTEAM,
			OP_MARINESFIELD,
			OP_OPFORFIELD,
			OP_MENUISOPEN,
			OP_WRITINGDATA,
			OP_INLOBBY,
			OP_INGAMELOBBY,
			OP_INPRIVATEPARTY,
			OP_INPRIVATEPARTYHOST,
			OP_PRIVATEPARTYHOSTINLOBBY,
			OP_ALONEINPARTY,
			OP_ADSJAVELIN,
			OP_WEAPLOCKBLINK,
			OP_WEAPATTACKTOP,
			OP_WEAPATTACKDIRECT,
			OP_WEAPLOCKING,
			OP_WEAPLOCKED,
			OP_WEAPLOCKTOOCLOSE,
			OP_WEAPLOCKSCREENPOSX,
			OP_WEAPLOCKSCREENPOSY,
			OP_SECONDSASTIME,
			OP_TABLELOOKUP,
			OP_TABLELOOKUPBYROW,
			OP_TABLEGETROWNUM,
			OP_LOCSTRING,
			OP_LOCALVARINT,
			OP_LOCALVARBOOL,
			OP_LOCALVARFLOAT,
			OP_LOCALVARSTRING,
			OP_TIMELEFT,
			OP_SECONDSASCOUNTDOWN,
			OP_GAMEMSGWNDACTIVE,
			OP_GAMETYPENAME,
			OP_GAMETYPE,
			OP_GAMETYPEDESCRIPTION,
			OP_SCOREATRANK,
			OP_SPECTATINGCLIENT,
			OP_SPECTATINGFREE,
			OP_KEYBINDING,
			OP_ACTIONSLOTUSABLE,
			OP_HUDFADE,
			OP_MAXRECOMMENDEDPLAYERS,
			OP_ACCEPTINGINVITE,
			OP_ISINTERMISSION,
			OP_GAMEHOST,
			OP_PARTYISMISSINGMAPPACK,
			OP_PARTYMISSINGMAPPACKERROR,
			OP_ANYNEWMAPPACKS,
			OP_AMISELECTED,
			OP_PARTYSTATUSSTRING,
			OP_ATTACHEDCONTROLLERCOUNT,
			OP_ISSPLITSCREENONLINEPOSSIBLE,
			OP_SPLITSCREENPLAYERCOUNT,
			OP_GETPLAYERDATA,
			OP_GETPLAYERDATASPLITSCREEN,
			OP_GETMATCHRULESDATA,
			OP_GETSAVEDMATCHRULESMETADATA,
			OP_LEVELFOREXPERIENCEMP,
			OP_LEVELFOREXPERIENCESO,
			OP_ISITEMUNLOCKED,
			OP_ISITEMUNLOCKEDSPLITSCREEN,
			OP_ISCARDICONUNLOCKED,
			OP_ISCARDTITLEUNLOCKED,
			OP_ISCARDICONNEW,
			OP_ISCARDTITLENEW,
			OP_ISCARDICONUNLOCKEDSPLITSCREEN,
			OP_ISCARDTITLEUNLOCKEDSPLITSCREEN,
			OP_ISCARDICONNEWSPLITSCREEN,
			OP_ISCARDTITLENEWSPLITSCREEN,
			OP_ISPROFILEITEMUNLOCKED,
			OP_ISPROFILEITEMUNLOCKEDSPLITSCREEN,
			OP_ISPROFILEITEMNEW,
			OP_ISPROFILEITEMNEWSPLITSCREEN,
			OP_DEBUGPRINT,
			OP_GETPLAYERDATAANYBOOLTRUE,
			OP_GETPROFILEANYBOOLTRUE,
			OP_WEAPONCLASSNEW,
			OP_WEAPONNAME,
			OP_ISRELOADING,
			OP_SAVEGAMEAVAILABLE,
			OP_UNLOCKEDITEMCOUNT,
			OP_UNLOCKEDITEMCOUNTSPLITSCREEN,
			OP_UNLOCKEDITEM,
			OP_UNLOCKEDITEMSPLITSCREEN,
			OP_RADARISJAMMED,
			OP_RADARJAMINTENSITY,
			OP_RADARISENABLED,
			OP_ISEMPJAMMED,
			OP_PLAYERADS,
			OP_WEAPONHEATACTIVE,
			OP_WEAPONHEATVALUE,
			OP_WEAPONHEATOVERHEATED,
			OP_GETSPLASHTEXT,
			OP_GETSPLASHDESCRIPTION,
			OP_GETSPLASHMATERIAL,
			OP_SPLASHHASICON,
			OP_SPLASHROWNUM,
			OP_GETFOCUSEDITEMNAME,
			OP_GETFOCUSEDITEMX,
			OP_GETFOCUSEDITEMY,
			OP_GETFOCUSEDITEMWIDTH,
			OP_GETFOCUSEDITEMHEIGHT,
			OP_GETITEMX,
			OP_GETITEMY,
			OP_GETITEMWIDTH,
			OP_GETITEMHEIGHT,
			OP_PLAYLIST,
			OP_SCOREBOARDEXTERNALMUTENOTICE,
			OP_GETCLIENTMATCHDATA,
			OP_GETCLIENTMATCHDATADEF,
			OP_GETMAPNAME,
			OP_GETMAPIMAGE,
			OP_GETMAPCUSTOM,
			OP_GETMIGRATIONSTATUS,
			OP_GETPLAYERCARDINFO,
			OP_ISOFFLINEPROFILESELECTED,
			OP_COOPPLAYER,
			OP_ISCOOP,
			OP_GETPARTYSTATUS,
			OP_GETSEARCHPARAMS,
			OP_GETTIMEPLAYED,
			OP_ISSELECTEDPLAYERFRIEND,
			OP_GETCHARBYINDEX,
			OP_GETPROFILEDATA,
			OP_GETPROFILEDATASPLITSCREEN,
			OP_ISPROFILESIGNEDIN,
			OP_GETWAITPOPUPSTATUS,
			OP_GETNATTYPE,
			OP_GETLOCALIZEDNATTYPE,
			OP_GETADJUSTEDSAFEAREAHORIZONTAL,
			OP_GETADJUSTEDSAFEAREAVERTICAL,
			OP_CONNECTIONINFO,
			OP_OFFLINEPROFILECANSAVE,
			OP_USERWITHOUTOFFLINEPROFILEWARNING,
			OP_ALLSPLITSCREENPROFILESCANSAVE,
			OP_ALLSPLITSCREENPROFILESARESIGNEDIN,
			OP_DOWEHAVEMAPPACK,
			OP_MAYINVITEPLAYERTOPARTY,
			OP_GETPATCHNOTES,
			OP_GETGAMEINFOS,
			OP_COOPREADY,
			OP_VOTECAST,
			OP_VOTEPASSED,
			OP_GETMAPVOTEMAPIMAGE,
			OP_GETMAPVOTEMAPNAME,
			OP_MAPVOTEGAMETYPENAME,
			OP_ISFRIENDINVITABLE,
			OP_ISFRIENDJOINABLE,
			OP_GETSORTEDCHALLENGEINDEX,
			OP_GETSORTEDCHALLENGENAME,
			OP_GETSORTEDCHALLENGECOUNT,
			OP_GETFILTERCHALLENGECOUNT,
			OP_GETFILTERCHALLENGELOCKEDCOUNT,
			OP_GETFILTERCHALLENGECOMPLETECOUNT,
			OP_ISSORTEDCHALLENGETIERED,
			OP_GETCHALLENGEFILTERCACHECOUNT,
			OP_GETCHALLENGEFILTERCACHECOMPLETECOUNT,
			OP_ISCOOPSEARCHING,
			OP_COOPSHOWPUBLICTYPE,
			OP_COOPDISPLAYABLEGROUPNUM,
			OP_COOPHASREQUIREDONLINEFILES,
			OP_GETTEXTWIDTH,
			OP_GETTEXTHEIGHT,
			OP_ISDEVELOPER,
			OP_ISUSINGAIRBURST,
			OP_GETAIRBURSTMETERS,
			OP_GETCROSSHAIRTRACEMETERS,
			OP_GETFACEBOOKSTATUSTEXT,
			OP_ISFACEBOOKLOGGEDIN,
			OP_ISFACEBOOKCHECKING,
			OP_ISFACEBOOKALLOWED,
			OP_GETPRIVATEPARTYSTATUS,
			OP_INCLUDEDINMAPROTATION,
			OP_SELECT,
			OP_ISDEMOPLAYING,
			OP_GETUSERGROUPTEXT,
			OP_GETUSERGROUPCOMMONINTERESTTOTAL,
			OP_ISDEMOFOLLOWCAMERA,
			OP_ISDEMOFREECAMERA,
			OP_ISDEMOCAPTURINGSCREENSHOT,
			OP_ISPARTYHOSTWAITINGONMEMBERS,
			OP_ISPOPUPPARTYMEMBERAWAY,
			OP_ISSELECTEDPARTYMEMBERAWAY,
			OP_GETTIME,
			OP_GAMEENDTIME,
			OP_HASFOCUS,
			OP_MENUHASFOCUS,
			OP_GETDEMOSEGMENTCOUNT,
			OP_GETDEMOSEGMENTINFORMATION,
			OP_ISCLIPMODIFIED,
			OP_ISUSINGMATCHRULESDATA,
			OP_ISGUEST,
			OP_GETFACEBOOKHELPTEXT,
			OP_ISELITECLANALLOWED,
			OP_ISENTITLEMENTSALLOWED,
			OP_ISUSERGROUPSALLOWED,
			OP_ISWAITINGFORONLINESERVICES,
			OP_GETTEXTWIDTHMODCASE,
			OP_GETSAVESCREENTITLE,
			OP_GETSAVESCREENDESCRIPTION,
			OP_GETONLINEVAULTSELECTEDITEMDATA,
			OP_ISONLINEVAULTRESTRICTED,
			OP_ISCONTENTSERVERTASKINPROGRESS,
			OP_GETCONTENTSERVERTASKPROGRESS,
			OP_GETRECENTGAMESSELECTEDITEMDATA,
			OP_GAMETYPENAMEABBREVIATED,
			OP_MAPVOTEGAMETYPENAMEABBREVIATED,
			OP_ISUSERSIGNEDINTOLIVE,
			OP_USERCANPLAYONLINE,
			OP_GETPASTTITLERANK,
			OP_GETFEEDERDATA,
			OP_PARTYCLIENTSUPTODATE,
			OP_TRUNCATETEXTWITHELLIPSIS,
			OP_UISTARTED,
			OP_CANRENDERCLIP,
			OP_GETPREVIEWMAPCUSTOM,
			OP_GETDLCMAPSAVAILABLECOUNT,
			OP_ISUSERSIGNEDIN,
			OP_ISUSINGINTERMISSIONTIMER,
			OP_ISUSINGCUSTOMMAPROTATION,
			OP_MENUISTOPMOST,
			OP_FACEBOOK_ISPLATFROMFRIEND,
			OP_ELITECLAN_ISPLATFROMFRIEND,
			OP_ELITECLAN_ISME,
			OP_ELITECLAN_ISLEADER,
			OP_ISUSERSIGNEDINFORVAULT,
			OP_GETUSINGMATCHRULESDATA,
			OP_CANUSERACCESSONLINEVAULT,
			OP_FRIEND_GETGAMERTAG,
			OP_RECENTPLAYER_GETGAMERTAG,
			OP_LIVEPARTY_GETGAMERTAG,
			OP_FACEBOOK_GETGAMERTAG,
			OP_ELITECLAN_GETGAMERTAG,
			OP_LIVEPARTY_ISME,
			OP_LIVEPARTY_ISLOCAL,
			OP_DOUBLECLICKWASRIGHTCLICK,
			OP_ISDEMOCLIPRECORDING,
			OP_GETINDEXFROMSTRING,
			OP_GETSTRINGWITHOUTINDEX,
			OP_ELITECLAN_GETNAME,
			OP_ELITECLAN_GETHELP,
			OP_ELITECLAN_GETMOTD,
			OP_ELITECLAN_ISMEMBER,
			OP_ELITECLAN_ISEMBLEM_OK,
			OP_FACEBOOK_FRIENDS_SHOW_NEXT,
			OP_FACEBOOK_FRIENDS_SHOW_PREV,
			OP_GETONLINEVAULTFRIENDGAMERTAG,
			OP_GETOBJECTIVELISTHEIGHT,
			OP_ISCLIENTDEMOENABLED,
			OP_ISUSERSIGNEDINTODEMONWARE,
			OP_CUSTOMCLASSISRESTRICTED,
			OP_WEAPONISRESTRICTED,
			OP_ANYSPLITSCREENPROFILESARESIGNEDIN,
			OP_ISGUESTSPLITSCREEN,
			OP_ISITEMUNLOCKEDBYCLIENT,
			OP_ISANYUSERSIGNEDINTOLIVE,
			OP_GETPASTTITLEPRESTIGE,
			OP_SPLITSCREENACTIVEGAMEPADCOUNT,
			OP_SHOWFRIENDPLAYERCARD,
			OP_GETFRIENDPLAYERCARDPRESENCE,
			OP_SHOWRECENTPLAYERGROUPICON,
			OP_GETWRAPPEDTEXTHEIGHT,
			OP_CANCLIENTSAVE,
			OP_GETGAMEINVITESCOUNT,
			OP_ISSPLITSCREENGAMERLIVEENABLED,
			OP_SO_COOPSHOWCOMMONGROUPICON,
			OP_STRIPCOLORSFROMSTRING,
			OP_DEPRECATED,
			OP_ISCHALLENGEPERIODIC,
			OP_GETCHALLENGEDATA,
			OP_ISELITEAPPPRESENT,
			OP_ELITECLAN_SELECTEDISME,
			OP_ENOUGHSTORAGESPACEFORCLIENTDEMO,
			OP_ISUSERSIGNEDINFORCOMMERCE,
			OP_GETFACEBOOKMENUTEXT,
			OP_GETFACEBOOKISPOSTING,
			OP_MEETPLAYER_ISPLATFORMFRIEND,
			OP_ISSELECTEDPLAYERGUEST,
			OP_GETSPLITSCREENCONTROLLERCLIENTNUM,
			OP_ISCLIENTDEMOENABLEDSPLITSCREEN,
			OP_ITEMCANTAKEFOCUS,
			OP_GETTIMESINCELASTDOUBLECLICK,
			OP_ISSERVERLISTREFRESHING,
			OP_ISRECIPENAMEVALID,
			OP_RECIPEEXISTS,
			OP_GETFACEBOOKOPTIONSHELPTEXT,
			OP_DOWEHAVEALLAVAILABLEMAPPACKS,
			OP_ISTHERENEWELITEITEMS,
			OP_ISPAYINGSUBSCRIBER,
			OP_LOCALUSER_ISMISSINGMAPPACK,
			OP_LOCALUSER_MISSINGMAPPACKERROR,
			OP_GETFIRSTSPECOPSDLCMAP,
			OP_LOCALUSER_MISSINGMAPNAME,
			OP_SHOWSTORENEW,
			OP_COMMERCE_GETSTATUS,
			OP_ISMANIFESTDOWNLOADED,
			OP_AREALLITEMSUNLOCKED,
			OP_COMMERCE_GETSTATUSCOND,
			OP_DOWEHAVEMISSINGOWNEDCONTENT
		};

		struct UIFunctionList
		{
			int totalFunctions;
			Statement_s** functions;
		};

		struct StaticDvar
		{
			void* dvar;
			char* dvarName;
		};

		struct StaticDvarList
		{
			int numStaticDvars;
			StaticDvar** staticDvars;
		};

		struct StringList
		{
			int totalStrings;
			const char** strings;
		};

		struct ExpressionSupportingData
		{
			UIFunctionList uifunctions;
			StaticDvarList staticDvarList;
			StringList uiStrings;
		};

		enum expDataType
		{
			VAL_INT = 0x0,
			VAL_FLOAT = 0x1,
			VAL_STRING = 0x2,
			NUM_INTERNAL_DATATYPES = 0x3,
			VAL_FUNCTION = 0x3,
			NUM_DATATYPES = 0x4,
		};

		struct ExpressionString
		{
			const char* string;
		};

		union operandInternalDataUnion
		{
			int intVal;
			float floatVal;
			ExpressionString stringVal;
			Statement_s* function;
		};

#pragma pack(push, 1)
		struct Operand
		{
			expDataType dataType;
			operandInternalDataUnion internals;
		};
#pragma pack(pop)

		union entryInternalData
		{
			operationEnum op;
			Operand operand;
		};

		struct expressionEntry
		{
			int type;
			entryInternalData data;
		};

		struct ExpressionPersistentState
		{
			int flags;
			int playerDataKey[4];
			int lastExecuteTime[4];
			Operand lastResult[4];
		};

		struct Statement_s
		{
			int numEntries;
			expressionEntry* entries;
			ExpressionSupportingData* supportingData;
			ExpressionPersistentState persistentState;
		};

		struct SetLocalVarData
		{
			const char* localVarName;
			Statement_s* expression;
		};

		struct ConditionalScript
		{
			MenuEventHandlerSet* eventHandlerSet;
			Statement_s* eventExpression; // loads this first
		};

		union EventData
		{
			const char* unconditionalScript;
			ConditionalScript* conditionalScript;
			MenuEventHandlerSet* elseScript;
			SetLocalVarData* setLocalVarData;
		};

#pragma pack(push, 4)
		struct MenuEventHandler
		{
			EventData eventData;
			EventType eventType;
		};
#pragma pack(pop)

		struct MenuEventHandlerSet
		{
			int eventHandlerCount;
			MenuEventHandler** eventHandlers;
		};

		struct ItemKeyHandler
		{
			int key;
			MenuEventHandlerSet* action;
			ItemKeyHandler* next;
		};

#pragma pack(push, 4)
		struct rectDef_s
		{
			float x;
			float y;
			float w;
			float h;
			char horzAlign;
			char vertAlign;
		};
#pragma pack(pop)

		// sizeof = 0xB0
		struct windowDef_t
		{
			const char* name;
			rectDef_s rect;
			rectDef_s rectClient;
			const char* group;
			int style;
			int border;
			int ownerDraw;
			int ownerDrawFlags;
			float borderSize;
			int staticFlags;
			int dynamicFlags;
			int nextTime;
			float foreColor[4];
			float backColor[4];
			float borderColor[4];
			float outlineColor[4];
			float disableColor[4];
			Material* background;
		};

		enum ItemFloatExpressionTarget
		{
			ITEM_FLOATEXP_TGT_RECT_X = 0x0,
			ITEM_FLOATEXP_TGT_RECT_Y = 0x1,
			ITEM_FLOATEXP_TGT_RECT_W = 0x2,
			ITEM_FLOATEXP_TGT_RECT_H = 0x3,
			ITEM_FLOATEXP_TGT_FORECOLOR_R = 0x4,
			ITEM_FLOATEXP_TGT_FORECOLOR_G = 0x5,
			ITEM_FLOATEXP_TGT_FORECOLOR_B = 0x6,
			ITEM_FLOATEXP_TGT_FORECOLOR_RGB = 0x7,
			ITEM_FLOATEXP_TGT_FORECOLOR_A = 0x8,
			ITEM_FLOATEXP_TGT_GLOWCOLOR_R = 0x9,
			ITEM_FLOATEXP_TGT_GLOWCOLOR_G = 0xA,
			ITEM_FLOATEXP_TGT_GLOWCOLOR_B = 0xB,
			ITEM_FLOATEXP_TGT_GLOWCOLOR_RGB = 0xC,
			ITEM_FLOATEXP_TGT_GLOWCOLOR_A = 0xD,
			ITEM_FLOATEXP_TGT_BACKCOLOR_R = 0xE,
			ITEM_FLOATEXP_TGT_BACKCOLOR_G = 0xF,
			ITEM_FLOATEXP_TGT_BACKCOLOR_B = 0x10,
			ITEM_FLOATEXP_TGT_BACKCOLOR_RGB = 0x11,
			ITEM_FLOATEXP_TGT_BACKCOLOR_A = 0x12,
			ITEM_FLOATEXP_TGT__COUNT = 0x13,
		};

		struct ItemFloatExpression
		{
			ItemFloatExpressionTarget target;
			Statement_s* expression;
		};

		struct editFieldDef_s
		{
			float minVal;
			float maxVal;
			float stepVal;
			float range;
			int maxChars;
			int maxCharsGotoNext;
			int maxPaintChars;
			int paintOffset;
		};

		struct multiDef_s
		{
			const char* dvarList[32];
			const char* dvarStr[32];
			float dvarValue[32];
			int count;
			int strDef;
		};

		struct columnInfo_s
		{
			int xpos;
			int ypos;
			int width;
			int height;
			int maxChars;
			int alignment;
		};

		// TODO: 4 bytes missing somewhere -->( added mousePos ) not sure if correct
		struct listBoxDef_s
		{
			int mousePos; // is this correct?
			int startPos[1];
			int endPos[1];
			int drawPadding;
			float elementWidth;
			float elementHeight;
			int elementStyle;
			int numColumns;
			columnInfo_s columnInfo[16];
			MenuEventHandlerSet* onDoubleClick;
			int notselectable;
			int noScrollBars;
			int usePaging;
			float selectBorder[4];
			Material* selectIcon;
			Statement_s* elementHeightExp;
		};

		struct newsTickerDef_s
		{
			int feedId;
			int speed;
			int spacing;
		};

		struct textScrollDef_s
		{
			int startTime;
		};

		union itemDefData_t
		{
			listBoxDef_s* listBox;
			editFieldDef_s* editField;
			multiDef_s* multi;
			const char* enumDvarName;
			newsTickerDef_s* ticker;
			textScrollDef_s* scroll;
			void* data;
		};

		struct __declspec(align(4)) itemDef_t
		{
			windowDef_t window;
			rectDef_s textRect;
			int type;
			int dataType;
			int alignment;
			int fontEnum;
			int textAlignMode;

			union
			{
				float floatArray1[3];

				struct
				{
					float textAlignX;
					float textAlignY;
					float textScale;
				};
			};

			int textStyle;
			int gameMsgWindowIndex;
			int gameMsgWindowMode;
			const char* text;
			int itemFlags;
			menuDef_t* parent;
			MenuEventHandlerSet* mouseEnterText;
			MenuEventHandlerSet* mouseExitText;
			MenuEventHandlerSet* mouseEnter;
			MenuEventHandlerSet* mouseExit;
			MenuEventHandlerSet* action;
			MenuEventHandlerSet* accept;
			MenuEventHandlerSet* onFocus;
			MenuEventHandlerSet* hasFocus;
			MenuEventHandlerSet* leaveFocus;
			const char* dvar;
			const char* dvarTest;
			ItemKeyHandler* onKey;
			const char* enableDvar;
			const char* localVar;
			int dvarFlags;
			snd_alias_list_t* focusSound;
			float special;
			int cursorPos;
			itemDefData_t typeData;
			int floatExpressionCount;
			ItemFloatExpression* floatExpressions;
			Statement_s* visibleExp;
			Statement_s* disabledExp;
			Statement_s* textExp;
			Statement_s* materialExp;
			float glowColor[4];
			bool decayActive;
			int fxBirthTime;
			int fxLetterTime;
			int fxDecayStartTime;
			int fxDecayDuration;
			int lastSoundPlayedTime;
			Statement_s* textAlignYExp;
		};

		struct menuTransition
		{
			int transitionType;
			int startTime;
			float startVal;
			float endVal;
			float time;
			int endTriggerType;
		};

		struct __declspec(align(4)) menuData_t
		{
			int fullScreen;
			int fadeCycle;

			union
			{
				float floatArray1[4];

				struct
				{
					float fadeClamp;
					float fadeAmount;
					float fadeInAmount;
					float blurWorld;
				};
			};

			MenuEventHandlerSet* onOpen;
			MenuEventHandlerSet* onCloseRequest;
			MenuEventHandlerSet* onClose;
			MenuEventHandlerSet* onESC;
			MenuEventHandlerSet* onFocusDueToClose;
			ItemKeyHandler* onKey;
			Statement_s* visibleExp;
			const char* allowedBinding;
			const char* soundName;
			float focusColor[4];
			Statement_s* rectXExp;
			Statement_s* rectYExp;
			Statement_s* rectWExp;
			Statement_s* rectHExp;
			Statement_s* openSoundExp;
			Statement_s* closeSoundExp;
			Statement_s* soundLoopExp;
			int cursorItem;
			menuTransition scaleTransition;
			menuTransition alphaTransition;
			menuTransition xTransition;
			menuTransition yTransition;
			ExpressionSupportingData* expressionData;
			char priority;
		};

		struct menuDef_t
		{
			menuData_t* data;
			windowDef_t window;
			int itemCount;
			itemDef_t** items;
		};

		struct MenuList
		{
			const char* name;
			int menuCount;
			menuDef_t** menus;
		};

		// Material stuff
		struct __declspec(align(4)) GfxImageLoadDef
		{
			char levelCount;
			char pad[3];
			int flags;
			int format;
			int resourceSize;
			char data[1];
		};

		struct Picmip
		{
			char platform[2];
		};

		struct CardMemory
		{
			int platform[2];
		};

		struct GfxImage
		{
			GfxImageLoadDef* texture;
			unsigned char mapType; // 5 is cube, 4 is 3d, 3 is 2d
			unsigned char semantic;
			unsigned char category;
			unsigned char flags;
			Picmip picmip;
			bool noPicmip;
			char track;
			CardMemory cardMemory;
			unsigned short width;
			unsigned short height;
			unsigned short depth;
			unsigned char levelCount;
			const char* name;
		};

		struct WaterWritable
		{
			float floatTime;
		};

		struct water_t
		{
			WaterWritable writable;
			float* H0X;
			float* H0Y;
			float* wTerm;
			int M;
			int N;
			float Lx;
			float Lz;
			float gravity;
			float windvel;
			float winddir[2];
			float amplitude;
			float codeConstant[4];
			GfxImage* image;
		};

		struct MaterialImage
		{
			unsigned int typeHash; // asset hash of type
			char firstCharacter; // first character of image name
			char secondLastCharacter; // second-last character of image name (maybe only in CoD4?!)
			unsigned char sampleState;
			unsigned char semantic;
			GfxImage* image; // Image* actually
		};

		struct VertexDecl
		{
			const char* name;
			int unknown;
			char pad[28];
			void* declarations[16];
		};

		struct PixelShader
		{
			const char* name;
			void* shader;
			DWORD* bytecode;
			int codeLen;
		};

		struct VertexShader
		{
			const char* name;
			void* shader;
			DWORD* bytecode;
			int codeLen;
		};

		struct MaterialArgumentCodeConst
		{
			unsigned __int16 index;
			char firstRow;
			char rowCount;
		};

		union MaterialArgumentDef
		{
			float* literalConst;
			MaterialArgumentCodeConst codeConst;
			unsigned int codeSampler;
			unsigned int nameHash;
		};

		struct ShaderArgumentDef
		{
			short type;
			short dest;
			MaterialArgumentDef u;
		};
#pragma pack(push, 4)
		struct MaterialPass
		{
			VertexDecl* vertexDecl;
			VertexShader* vertexShader;
			PixelShader* pixelShader;
			char perPrimArgCount;
			char perObjArgCount;
			char stableArgCount;
			char customSamplerFlags;
			ShaderArgumentDef* argumentDef;
		};
#pragma pack(pop)
		struct MaterialTechniqueHeader
		{
			const char* name;
			unsigned __int16 flags;
			unsigned __int16 passCount;
		};

		struct MaterialTechnique
		{
			MaterialTechniqueHeader hdr;
			MaterialPass pass[1];
		};

		struct MaterialTechniqueSet
		{
			const char* name;
			int pad;
			MaterialTechniqueSet* remappedTechniques;
			MaterialTechnique* techniques[54];
		};

		struct MaterialConstantDef
		{
			unsigned int nameHash;
			char name[12];
			float literal[4];
		};

		struct GfxStateBits
		{
			unsigned int loadBits[2];
		};

		enum GfxCameraRegionType : std::uint16_t
		{
			CAMERA_REGION_LIT_OPAQUE = 0x0,
			CAMERA_REGION_LIT_TRANS = 0x1,
			CAMERA_REGION_EMISSIVE = 0x2,
			CAMERA_REGION_DEPTH_HACK = 0x3,
			CAMERA_REGION_LIGHT_MAP_OPAQUE = 0x4,
			CAMERA_REGION_COUNT = 0x5,
			CAMERA_REGION_NONE = 0x5,
		};

		struct Material
		{
			const char* name; // 0
			unsigned char gameFlags;
			unsigned char sortKey;
			unsigned char animationX; // 6 // amount of animation frames in X
			unsigned char animationY; // 7 // amount of animation frames in Y
			int subRendererIndex; // 8 // 0x00
			unsigned int rendererIndex; // 12 // only for 3D models
			unsigned int surfaceTypeBits; //+20
			int unknown;
			char stateBitsEntry[54];
			unsigned char numMaps;
			unsigned char constantCount;
			unsigned char stateBitsCount;
			unsigned char stateFlags; // 0x03
			unsigned short cameraRegion; // 0x04
			MaterialTechniqueSet* techniqueSet; // '2d' techset
			MaterialImage* maps; // map references
			MaterialConstantDef* constantTable;
			GfxStateBits* stateMap; // might be NULL, need to test
			int NiceMemeIW;
			// int fakePointer;
		};

		// XModel
		struct XSurfaceVertexInfo
		{
			__int16 vertCount[4];
			unsigned __int16* vertsBlend;
		};

		union GfxColor
		{
			unsigned int packed;
			unsigned char array[4];
		};

		union PackedTexCoords
		{
			unsigned int packed;
			unsigned char array[4];
		};

		union PackedUnitVec
		{
			unsigned int packed;
			unsigned char array[4];
		};

		struct GfxPackedVertex
		{
			float xyz[3];
			float binormalSign;
			GfxColor color;
			PackedTexCoords texCoord;
			PackedUnitVec normal;
			PackedUnitVec tangent;
		};

		struct XSurfaceCollisionAabb
		{
			unsigned __int16 mins[3];
			unsigned __int16 maxs[3];
		};

		struct XSurfaceCollisionNode
		{
			XSurfaceCollisionAabb aabb;
			unsigned __int16 childBeginIndex;
			unsigned __int16 childCount;
		};

		struct XSurfaceCollisionLeaf
		{
			unsigned __int16 triangleBeginIndex;
		};

		struct XSurfaceCollisionTree
		{
			float trans[3];
			float scale[3];
			unsigned int nodeCount;
			XSurfaceCollisionNode* nodes;
			unsigned int leafCount;
			XSurfaceCollisionLeaf* leafs;
		};

		struct XRigidVertList
		{
			unsigned __int16 boneOffset;
			unsigned __int16 vertCount;
			unsigned __int16 triOffset;
			unsigned __int16 triCount;
			XSurfaceCollisionTree* collisionTree;
		};

		struct unknownUnion
		{
			GfxPackedVertex* packedVerticies;
			char* unknown1; //Size = ((vertCount << 1) + vertCount) << 3
			char* unknown2; //Size = ((vertCount << 2) + vertCount) << 2
		};

		struct Face
		{
			unsigned short v1;
			unsigned short v2;
			unsigned short v3;
		};
#pragma pack(push, 1)
		struct XSurfaceCTEntry
		{
			char pad[24];
			int numNode;
			char* node; // el size 16
			int numLeaf;
			short* leaf;
		};

		struct XSurfaceCT
		{
			int pad;
			int pad2;
			XSurfaceCTEntry* entry;
		};

		enum XSurfaceFlags : std::uint8_t
		{
			SURF_FLAG_VERTCOL_GREY = 0x8,
			SURF_FLAG_VERTCOL_NONE = 0x10,
			SURF_FLAG_QUANTIZED = 0x20,
			SURF_FLAG_SKINNED = 0x40,
		};

#pragma pack(push, 4)
		struct XSurface
		{
			char tileMode; // +0
			char flags; // +1			
			unsigned short vertCount; // +2
			unsigned short triCount; // +4
			unsigned char streamHandle; // +6
			unsigned char zoneHandle; // +7
			unsigned __int16 baseTriIndex;
			unsigned __int16 baseVertIndex;
			int unknown;
			Face* triIndices; // +12
			XSurfaceVertexInfo vertexInfo; // +16
			GfxPackedVertex* verticies; // +28
			int vertListCount; // +32
			XRigidVertList* rigidVertLists; // +36
			int partBits[6]; // +40
		};
#pragma pack(pop)

		struct ModelSurface
		{
			char* name;
			XSurface* xSurficies;
			short xSurficiesCount;
			short _pad;
			int partBits[6];
		};

		struct XSurfaceLod
		{
			float dist;
			short numSurfacesInLod;
			short surfIndex;
			ModelSurface* surfaces;
			int partBits[6];
			XSurface* surfs;
			char lod;
			char smcBaseIndexPlusOne;
			char smcSubIndexMask;
			char smcBucket;
		};

		struct XModelCollTri_s
		{
			float plane[4];
			float svec[4];
			float tvec[4];
		};

		struct XModelCollSurf_s
		{
			XModelCollTri_s* tris;
			int numCollTris;
			float mins[3];
			float maxs[3];
			int boneIdx;
			int contents;
			int surfFlags;
		};

		struct XBoneInfo
		{
			float bounds[2][3];
			float radiusSquared;
		};

		struct cplane_s
		{
			float normal[3];
			float dist;
			char type;
			char pad[3];
		};

		struct cplane_t
		{
			char pad[20];
		};

		struct cbrushside_t
		{
			cplane_s* plane;
			unsigned __int16 materialNum;
			char firstAdjacentSideOffset;
			char edgeCount;
		};

		struct BrushWrapper
		{
			float mins[3];
			float maxs[3];
			unsigned int numPlaneSide;
			cbrushside_t* side;
			char* edge;
			__int16 axialMaterialNum[2][3];
			__int16 firstAdjacentSideOffsets[2][3];
			int numEdge;
			cplane_s* plane;
		};

		//struct BrushWrapper
		//{
		//	char pad[24]; // 24
		//	short numPlaneSide; // 2
		//	short pad2; // 2
		//	cbrushside_t* side; // 4
		//	char* edge; // 4
		//	char pad3[24]; // 24
		//	int numEdge; // 4
		//	cplane_s* plane; // 4
		//}; // 68 bytes total
		struct PhysGeomInfo
		{
			BrushWrapper* brush;
			char pad[64];
		};

		struct PhysCollmap
		{
			char* name;
			int numInfo;
			PhysGeomInfo* info;
			char pad2[60];
		};

		enum PhysPresetScaling : __int32
		{
			PHYSPRESET_SCALING_LINEAR = 0x0,
			PHYSPRESET_SCALING_QUADRATIC = 0x1,
			PHYSPRESET_SCALING_COUNT = 0x2,
		};

#pragma pack(push, 4)
		struct PhysPreset
		{
			const char* name;
			int type;
			float mass;
			float bounce;
			float friction;
			float bulletForceScale;
			float explosiveForceScale;
			const char* sndAliasPrefix;
			float piecesSpreadFraction;
			float piecesUpwardVelocity;
			float minMomentum;
			float maxMomentum;
			float minPitch;
			float maxPitch;
			PhysPresetScaling volumeType;
			PhysPresetScaling pitchType;
			bool tempDefaultToCylinder;
			bool perSurfaceSndAlias;
		};
#pragma pack(pop)

#pragma pack(push, 1)
		// Only those structs are correct.
		struct XModelAngle
		{
			short x;
			short y;
			short z;
			short base;
		};

		struct XModelTagPos
		{
			float x;
			float y;
			float z;
		};

		struct DObjAnimMat
		{
			float quat[4];
			float trans[3];
			float transWeight;
		};

		struct Bounds
		{
			vec3_t midPoint;
			vec3_t halfSize;
		};

		struct XModel
		{
			char* name;
			char numBones;
			char numRootBones;
			unsigned char numSurfaces;
			char lodRampType;
			float scale;
			unsigned int noScalePartBits[6];
			short* boneNames;
			unsigned char* parentList;
			XModelAngle* tagAngles;
			XModelTagPos* tagPositions;
			char* partClassification;
			DObjAnimMat* animMatrix;
			Material** materials;
			XSurfaceLod lods[4];
			char maxLoadedLod;
			char numLods;
			char collLod;
			char flags;
			XModelCollSurf_s* colSurf;
			int numColSurfs;
			int contents;
			XBoneInfo* boneInfo;
			float radius;
			Bounds bounds;
			int memUsage;
			bool bad;
			char pad5[3];
			PhysPreset* physPreset;
			PhysCollmap* physCollmap;
			int unk;
		}; // total size 308
#pragma pack(pop)

		// Sound curve
#pragma pack(push, 4)
		struct SndCurve
		{
			union
			{
				const char* filename;
				const char* name;
			};

			unsigned __int16 knotCount;
			vec2_t knots[16];
		};
#pragma pack(pop)

		// Loaded sound
        struct _AILSOUNDINFO
        {
            int format;
            const void* data_ptr;
            unsigned int data_len;
            unsigned int rate;
            int bits;
            int channels;
            unsigned int samples;
            unsigned int block_size;
            const void* initial_ptr;
        };

#pragma pack(push, 4)
		struct MssSound
		{
            _AILSOUNDINFO info;
            char* data;
		};
#pragma pack(pop)

		struct LoadedSound
		{
			const char* name;
			MssSound sound;
		};

		// Sounds
		struct SpeakerLevels
		{
			int speaker;
			int numLevels;
			float levels[2];
		};

		struct ChannelMap
		{
			int entryCount; // how many entries are used
			SpeakerLevels speakers[6];
		};

		struct SpeakerMap
		{
			bool isDefault;
			char _pad[3];
			const char* name;
			ChannelMap channelMaps[2][2];
		};

		enum snd_alias_type_t : char
		{
			SAT_UNKNOWN = 0x0,
			SAT_LOADED = 0x1,
			SAT_STREAMED = 0x2,
			SAT_PRIMED = 0x3,
			SAT_COUNT = 0x4,
		};

		struct StreamFileNamePacked
		{
			unsigned __int64 offset;
			unsigned __int64 length;
		};

		struct StreamFileNameRaw
		{
			const char* dir;
			const char* name;
		};

		union StreamFileInfo
		{
			StreamFileNameRaw raw;
			StreamFileNamePacked packed;
		};

		struct StreamFileName
		{
			unsigned __int16 isLocalized;
			unsigned __int16 fileIndex;
			StreamFileInfo info;
		};

		/*struct StreamedSound
		{
		StreamFileName filename;
		unsigned int totalMsec;
		};*/
		struct StreamedSound
		{
			const char* dir;
			const char* name;
		};

		struct PrimedSound
		{
			StreamFileName filename;
			LoadedSound* loadedPart;
			int dataOffset;
			int totalSize;
			unsigned int primedCrc;
		};

		union SoundData
		{
			LoadedSound* loadSnd; // SoundFile->type == SAT_LOADED
			StreamedSound streamSnd; // SoundFile->type == SAT_STREAMED
			//PrimedSound primedSnd;	// SoundFile->type == SAT_PRIMED
		};

		struct SoundFile // 0x10
		{
			char type;
			bool exists;
			char _pad[2];
			SoundData sound;
		};

#pragma pack(push, 4)
		struct snd_alias_t
		{
			const char* aliasName;
			const char* subtitle;
			const char* secondaryAliasName;
			const char* chainAliasName;
			const char* mixerGroup;
			SoundFile* soundFile;
			int sequence;
			float volMin;
			float volMax;
			int volModIndex;
			float pitchMin;
			float pitchMax;
			float distMin;
			float distMax;
			float velocityMin;
			int flags;
			char masterPriority;
			float masterPercentage;
			float slavePercentage;
			float probability;
			float lfePercentage;
			float centerPercentage;
			int startDelay;
			SndCurve* volumeFalloffCurve;
			float envelopMin;
			float envelopMax;
			float envelopPercentage;
			SpeakerMap* speakerMap;
		};
#pragma pack(pop)

		struct snd_alias_list_t
		{
			union
			{
				const char* aliasName;
				const char* name;
			};

			snd_alias_t* head;
			int count;
		};

		// Tracers
		struct TracerDef
		{
			const char* name;
			Material* material;
			unsigned int drawInterval;
			float speed;
			float beamLength;
			float beamWidth;
			float screwRadius;
			float screwDist;
			float colors[5][4];
		};

		// Weapons
		enum weaponIconRatioType_t : int
		{
			WEAPON_ICON_RATIO_1TO1 = 0x0,
			WEAPON_ICON_RATIO_2TO1 = 0x1,
			WEAPON_ICON_RATIO_4TO1 = 0x2,
			WEAPON_ICON_RATIO_COUNT = 0x3,
		};

		enum ammoCounterClipType_t : int
		{
			AMMO_COUNTER_CLIP_NONE = 0x0,
			AMMO_COUNTER_CLIP_MAGAZINE = 0x1,
			AMMO_COUNTER_CLIP_SHORTMAGAZINE = 0x2,
			AMMO_COUNTER_CLIP_SHOTGUN = 0x3,
			AMMO_COUNTER_CLIP_ROCKET = 0x4,
			AMMO_COUNTER_CLIP_BELTFED = 0x5,
			AMMO_COUNTER_CLIP_ALTWEAPON = 0x6,
			AMMO_COUNTER_CLIP_COUNT = 0x7,
		};

		enum weapProjExplosion_t
		{
			WEAPPROJEXP_GRENADE = 0x0,
			WEAPPROJEXP_ROCKET = 0x1,
			WEAPPROJEXP_FLASHBANG = 0x2,
			WEAPPROJEXP_NONE = 0x3,
			WEAPPROJEXP_DUD = 0x4,
			WEAPPROJEXP_SMOKE = 0x5,
			WEAPPROJEXP_HEAVY = 0x6,
			WEAPPROJEXP_NUM = 0x7,
		};

		struct AdsOverlayStruct
		{
			Material* adsOverlayShader;
			Material* adsOverlayShaderLowRes;
			Material* adsOverlayShaderEMP;
			Material* adsOverlayShaderEMPLowRes;
			int adsOverlayReticle;
			float adsOverlayWidth;
			float adsOverlayHeight;
			float adsOverlayWidthSplitscreen;
			float adsOverlayHeightSplitscreeen;
		};

		union XAnimIndices
		{
			char* _1;
			unsigned __int16* _2;
			void* data;
		};

		union XAnimDynamicFrames
		{
			char (*_1)[3];
			unsigned __int16 (*_2)[3];
		};

		union XAnimDynamicIndices
		{
			char _1[1];
			unsigned __int16 _2[1];
		};

		struct XAnimPartTransFrames
		{
			float mins[3];
			float size[3];
			XAnimDynamicFrames frames;
			XAnimDynamicIndices indices;
		};

		union XAnimPartTransData
		{
			XAnimPartTransFrames frames;
			float frame0[3];
		};

		struct XAnimPartTrans
		{
			unsigned __int16 size;
			char smallTrans;
			__declspec(align(2)) XAnimPartTransData u;
		};

#pragma pack(push, 4)
		struct XAnimDeltaPartQuatDataFrames2
		{
			__int16(*frames)[2];
			XAnimDynamicIndices indices;
		};

		union XAnimDeltaPartQuatData2
		{
			XAnimDeltaPartQuatDataFrames2 frames;
			__int16 frame0[2];
		};

		struct XAnimDeltaPartQuat2
		{
			unsigned __int16 size;
			XAnimDeltaPartQuatData2 u;
		};

		struct XAnimDeltaPartQuatDataFrames
		{
			__int16(*frames)[4];
			XAnimIndices indices;
		};

		union XAnimDeltaPartQuatData
		{
			XAnimDeltaPartQuatDataFrames frames;
			__int16 frame0[4];
		};

		struct XAnimDeltaPartQuat
		{
			unsigned __int16 size;
			XAnimDeltaPartQuatData u;
		};
		
		struct XAnimDeltaPart
		{
			XAnimPartTrans* trans;
			XAnimDeltaPartQuat2* quat2;
			XAnimDeltaPartQuat* quat;
		};

		struct XAnimNotifyInfo
		{
			short name;
			float time;
		};

		struct XAnimParts
		{
			char* name; // 0
			unsigned short dataByteCount; // 4
			unsigned short dataShortCount; // 6
			unsigned short dataIntCount; // 8
			unsigned short randomDataByteCount; // 10 - 0xA
			unsigned short randomDataIntCount; // 12 - 0xC
			unsigned short framecount; // 14 - 0xE
			char flags; // 16
			char boneCount[10]; // 17
			char notetrackCount; // 27
			bool bLoop; // 28
			bool bDelta; // 29
			char assetType; // 30
			char ikType; // 31
			unsigned int randomDataShortCount; // 32 - 0x20
			unsigned int indexcount; // 36 - 0x24
			float framerate; // 40 - 0x28
			float frequency; // 44 - 0x2C
			unsigned short* tagnames; // 48 - 0x30
			char* dataByte; // 52 - 0x34
			short* dataShort; // 56 - 0x38
			int* dataInt; // 60 - 0x3C
			short* randomDataShort; // 64 - 0x40
			char* randomDataByte; // 68 - 0x44
			int* randomDataInt; // 72 - 0x48
			XAnimIndices indices; // 76 - 0x4C
			XAnimNotifyInfo* notetracks; // 80 - 0x50
			XAnimDeltaPart* delta; // 84 - 0x54
		};

		// StructuredDataDef
		enum StructuredDataTypeCategory
		{
			DATA_INT = 0x0,
			DATA_BYTE = 0x1,
			DATA_BOOL = 0x2,
			DATA_STRING = 0x3,
			DATA_ENUM = 0x4,
			DATA_STRUCT = 0x5,
			DATA_INDEXED_ARRAY = 0x6,
			DATA_ENUM_ARRAY = 0x7,
			DATA_FLOAT = 0x8,
			DATA_SHORT = 0x9,
			DATA_COUNT = 0xA,
		};
#pragma pack(push,4)
		struct StructuredDataEnumEntry
		{
			const char* name;
			unsigned __int16 index;
		};
#pragma pack(pop)
		struct StructuredDataEnum
		{
			int entryCount;
			int reservedEntryCount;
			StructuredDataEnumEntry* entries;
		};

		union StructuredDataTypeUnion
		{
			unsigned int stringDataLength;
			int enumIndex;
			int structIndex;
			int indexedArrayIndex;
			int enumedArrayIndex;
		};

		struct StructuredDataType
		{
			StructuredDataTypeCategory type;
			StructuredDataTypeUnion u;
		};
#pragma pack(push,4)
		struct StructuredDataStructProperty
		{
			const char* name;
			StructuredDataType item;
			int offset;
			int validation;
		};
#pragma pack(pop)
		struct StructuredDataStruct
		{
			int propertyCount;
			StructuredDataStructProperty* properties;
			int size;
			unsigned int bitOffset;
		};

		struct StructuredDataIndexedArray
		{
			int arraySize;
			StructuredDataType elementType;
			unsigned int elementSize;
		};

		struct StructuredDataEnumedArray
		{
			int enumIndex;
			StructuredDataType elementType;
			unsigned int elementSize;
		};

		struct StructuredDataDef
		{
			int version;
			unsigned int formatChecksum;
			int enumCount;
			StructuredDataEnum* enums;
			int structCount;
			StructuredDataStruct* structs;
			int indexedArrayCount;
			StructuredDataIndexedArray* indexedArrays;
			int enumedArrayCount;
			StructuredDataEnumedArray* enumedArrays;
			StructuredDataType rootType;
			unsigned int size;
		};

		struct StructuredDataDefSet
		{
			const char* name;
			unsigned int defCount;
			StructuredDataDef* defs;
		};

		// MapEnts
		struct TriggerModel
		{
			int contents;
			unsigned short hullCount;
			unsigned short firstHull;
		};

		struct TriggerHull
		{
			Bounds bounds;
			int contents;
			unsigned short slabCount;
			unsigned short firstSlab;
		};

		struct TriggerSlab
		{
			vec3_t dir;
			float midPoint;
			float halfSize;
		};

		struct MapTriggers
		{
			int modelCount;
			TriggerModel* models; // sizeof 8
			int hullCount;
			TriggerHull* hulls; // sizeof 32
			int slabCount;
			TriggerSlab* slabs; // sizeof 20
		};

		struct ClientTriggerAabbNode
		{
			Bounds bounds;
			unsigned __int16 firstChild;
			unsigned __int16 childCount;
		};

		struct ClientTriggers
		{
			MapTriggers trigger;
			unsigned __int16 numClientTriggerNodes;
			ClientTriggerAabbNode* clientTriggerAabbTree;
			unsigned int triggerStringLength;
			char* triggerString;
			__int16* visionSetTriggers;
			char* triggerType;
			float* origins;
			float* scriptDelay;
			__int16* audioTriggers;
		};

		struct MapEnts
		{
			const char* name;
			char* entityString;
			int numEntityChars;
			MapTriggers trigger;
			ClientTriggers clientTrigger;
		};
#pragma pack(pop)

		// FxEffectDef
		/* FxEffectDef::flags */
#define FX_ELEM_LOOPING				0x1
#define FX_ELEM_USE_RAND_COLOR			0x2
#define FX_ELEM_USE_RAND_ALPHA			0x4
#define FX_ELEM_USE_RAND_SIZE0			0x8
#define FX_ELEM_USE_RAND_SIZE1			0x10
#define FX_ELEM_USE_RAND_SCALE			0x20
#define FX_ELEM_USE_RAND_ROT_DELTA		0x40
#define FX_ELEM_MOD_COLOR_BY_ALPHA		0x80
#define FX_ELEM_USE_RAND_VEL0			0x100
#define FX_ELEM_USE_RAND_VEL1			0x200
#define FX_ELEM_USE_BACK_COMPAT_VEL		0x400
#define FX_ELEM_ABS_VEL0			0x800
#define FX_ELEM_ABS_VEL1			0x1000
#define FX_ELEM_PLAY_ON_TOUCH			0x2000
#define FX_ELEM_PLAY_ON_DEATH			0x4000
#define FX_ELEM_PLAY_ON_RUN			0x8000
#define FX_ELEM_BOUNDING_SPHERE			0x10000
#define FX_ELEM_USE_ITEM_CLIP			0x20000
#define FX_ELEM_DISABLED			0x80000000
#define FX_ELEM_DECAL_FADE_IN			0x40000

		/* FxElemDef::flags */
		//#define FX_ELEM_SPAWN_RELATIVE_TO_EFFECT	0x2
		//#define FX_ELEM_SPAWN_FRUSTUM_CULL		0x4
		//#define FX_ELEM_RUNNER_USES_RAND_ROT		0x8
		//#define FX_ELEM_SPAWN_OFFSET_NONE		0x0
		//#define FX_ELEM_SPAWN_OFFSET_SPHERE		0x10
		//#define FX_ELEM_SPAWN_OFFSET_CYLINDER		0x20
		//#define FX_ELEM_SPAWN_OFFSET_MASK		0x30
		//#define FX_ELEM_RUN_RELATIVE_TO_WORLD		0x0
		//#define FX_ELEM_RUN_RELATIVE_TO_SPAWN		0x40
		//#define FX_ELEM_RUN_RELATIVE_TO_EFFECT		0x80
		//#define FX_ELEM_RUN_RELATIVE_TO_OFFSET		0xC0
		//#define FX_ELEM_RUN_MASK			0xC0
		//#define FX_ELEM_USE_COLLISION			0x100
		//#define FX_ELEM_DIE_ON_TOUCH			0x200
		//#define FX_ELEM_DRAW_PAST_FOG			0x400
		//#define FX_ELEM_DRAW_WITH_VIEWMODEL		0x800
		//#define FX_ELEM_BLOCK_SIGHT			0x1000
		//#define FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY	0x2000
		//#define FX_ELEM_TRAIL_ORIENT_BY_VELOCITY	0x4000
		//#define FX_ELEM_EMIT_ORIENT_BY_ELEM		0x8000
		//#define FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL	0x1000000
		//#define FX_ELEM_HAS_VELOCITY_GRAPH_WORLD	0x2000000
		//#define FX_ELEM_HAS_GRAVITY			0x4000000
		//#define FX_ELEM_USE_MODEL_PHYSICS		0x8000000
		//#define FX_ELEM_NONUNIFORM_SCALE		0x10000000
		//#define FX_ELEM_CLOUD_SHAPE_CUBE		0x0
		//#define FX_ELEM_CLOUD_SHAPE_SPHERE_LARGE	0x20000000
		//#define FX_ELEM_CLOUD_SHAPE_SPHERE_MEDIUM	0x40000000
		//#define FX_ELEM_CLOUD_SHAPE_SPHERE_SMALL	0x60000000
		//#define FX_ELEM_CLOUD_MASK			0x60000000
		//#define FX_ELEM_FOUNTAIN_DISABLE_COLLISION	0x80000000

		/* FxElemAtlas::behavior */
#define FX_ATLAS_START_MASK		0x3
#define FX_ATLAS_START_FIXED		0x0
#define FX_ATLAS_START_RANDOM		0x1
#define FX_ATLAS_START_INDEXED		0x2
#define FX_ATLAS_PLAY_OVER_LIFE		0x4
#define FX_ATLAS_LOOP_ONLY_N_TIMES	0x8

		enum FxElemType : char
		{
			FX_ELEM_TYPE_SPRITE_BILLBOARD = 0x0,
			FX_ELEM_TYPE_SPRITE_ORIENTED = 0x1,
			FX_ELEM_TYPE_TAIL = 0x2,
			FX_ELEM_TYPE_TRAIL = 0x3,
			FX_ELEM_TYPE_CLOUD = 0x4,
			FX_ELEM_TYPE_SPARKCLOUD = 0x5,
			FX_ELEM_TYPE_SPARKFOUNTAIN = 0x6,
			FX_ELEM_TYPE_MODEL = 0x7,
			FX_ELEM_TYPE_OMNI_LIGHT = 0x8,
			FX_ELEM_TYPE_SPOT_LIGHT = 0x9,
			FX_ELEM_TYPE_SOUND = 0xA,
			FX_ELEM_TYPE_DECAL = 0xB,
			FX_ELEM_TYPE_RUNNER = 0xC,
			FX_ELEM_TYPE_COUNT = 0xD,
			FX_ELEM_TYPE_LAST_SPRITE = 0x3,
			FX_ELEM_TYPE_LAST_DRAWN = 0x9,
		};

		struct FxElemVec3Range
		{
			float base[3];
			float amplitude[3];
		};

		struct FxIntRange
		{
			int base;
			int amplitude;
		};

		struct FxFloatRange
		{
			float base;
			float amplitude;
		};

		struct FxSpawnDefLooping
		{
			int intervalMsec;
			int count;
		};

		struct FxSpawnDefOneShot
		{
			FxIntRange count;
		};

		union FxSpawnDef
		{
			FxSpawnDefLooping looping;
			FxSpawnDefOneShot oneShot;
		};

		struct FxElemMarkVisuals
		{
			Material* materials[2];
		};

		struct FxEffectDef;

		union FxEffectDefRef
		{
			FxEffectDef* handle;
			const char* name;
		};

		struct GfxLightDef;
		union FxElemVisuals
		{
			const void* anonymous;
			GfxLightDef* lightDef;
			Material* material;
			XModel* xmodel;
			FxEffectDefRef* effectDef;
			const char* soundName;
		};

		union FxElemDefVisuals
		{
			FxElemMarkVisuals* markArray;
			FxElemVisuals* array;
			FxElemVisuals instance;
		};

		struct FxTrailVertex
		{
			float pos[2];
			float normal[2];
			float texCoord[2];
		};

		struct FxTrailDef
		{
			int scrollTimeMsec;
			int repeatDist;
			float invSplitDist;
			float invSplitArcDist;
			float invSplitTime;
			int vertCount;
			FxTrailVertex* verts;
			int indCount;
			unsigned __int16* inds;
		};

		struct FxSparkFountainDef
		{
			float gravity;
			float bounceFrac;
			float bounceRand;
			float sparkSpacing;
			float sparkLength;
			int sparkCount;
			float loopTime;
			float velMin;
			float velMax;
			float velConeFrac;
			float restSpeed;
			float boostTime;
			float boostFactor;
		};

		struct FxSpotLightDef
		{
			float fovInnerFraction;
			float startRadius;
			float endRadius;
			float brightness;
			float maxLength;
			int exponent;
		};

		union FxElemExtendedDefPtr
		{
			char* unknownDef;
			FxSparkFountainDef* sparkFountain;
			FxTrailDef* trailDef;
			FxSpotLightDef* spotLightDef;
		};

		struct FxElemAtlas
		{
			char behavior;
			char index;
			char fps;
			char loopCount;
			char colIndexBits;
			char rowIndexBits;
			__int16 entryCount;
		};

		struct FxElemVelStateInFrame
		{
			FxElemVec3Range velocity;
			FxElemVec3Range totalDelta;
		};

		struct FxElemVelStateSample
		{
			FxElemVelStateInFrame local;
			FxElemVelStateInFrame world;
		};

		struct FxElemVisualState
		{
			unsigned char color[4];
			float rotationDelta;
			float rotationTotal;
			float size[2];
			float scale;
		};

		struct FxElemVisStateSample
		{
			FxElemVisualState base;
			FxElemVisualState amplitude;
		};

		struct FxFloatRange3
		{
			FxFloatRange range[3];
		};

		/* 515 */
		enum FxElemDefFlagTypes
		{
			FX_FLAG_TYPE_EDITOR = 0x0,
			FX_FLAG_TYPE_NATIVE = 0x1,
			FX_FLAG_TYPE_ATLAS = 0x2,
			FX_FLAG_TYPE_COUNT = 0x3,
		};

		/* 516 */
		enum FxElemDefEditorFlags
		{
			FX_ED_FLAG_LOOPING = 0x1,
			FX_ED_FLAG_USE_RANDOM_COLOR = 0x2,
			FX_ED_FLAG_USE_RANDOM_ALPHA = 0x4,
			FX_ED_FLAG_USE_RANDOM_SIZE_0 = 0x8,
			FX_ED_FLAG_USE_RANDOM_SIZE_1 = 0x10,
			FX_ED_FLAG_USE_RANDOM_SCALE = 0x20,
			FX_ED_FLAG_USE_RANDOM_ROTATION_DELTA = 0x40,
			FX_ED_FLAG_MODULATE_COLOR_BY_ALPHA = 0x80,
			FX_ED_FLAG_USE_RANDOM_VELOCITY_0 = 0x100,
			FX_ED_FLAG_USE_RANDOM_VELOCITY_1 = 0x200,
			FX_ED_FLAG_BACKCOMPAT_VELOCITY = 0x400,
			FX_ED_FLAG_ABSOLUTE_VELOCITY_0 = 0x800,
			FX_ED_FLAG_ABSOLUTE_VELOCITY_1 = 0x1000,
			FX_ED_FLAG_PLAY_ON_TOUCH = 0x2000,
			FX_ED_FLAG_PLAY_ON_DEATH = 0x4000,
			FX_ED_FLAG_PLAY_ON_RUN = 0x8000,
			FX_ED_FLAG_BOUNDING_SPHERE = 0x10000,
			FX_ED_FLAG_USE_ITEM_CLIP = 0x20000,
			FX_ED_FLAG_FADE_IN_DECAL = 0x40000,
			FX_ED_FLAG_DISABLED = 0x80000000,
		};

		enum FxElemDefFlags : std::uint32_t
		{
			FX_ELEM_SPAWN_RELATIVE_TO_EFFECT = 0x2,
			FX_ELEM_SPAWN_FRUSTUM_CULL = 0x4,
			FX_ELEM_RUNNER_USES_RAND_ROT = 0x8,
			FX_ELEM_SPAWN_OFFSET_NONE = 0x0,
			FX_ELEM_SPAWN_OFFSET_SPHERE = 0x10,
			FX_ELEM_SPAWN_OFFSET_CYLINDER = 0x20,
			FX_ELEM_SPAWN_OFFSET_MASK = 0x30,
			FX_ELEM_RUN_RELATIVE_TO_WORLD = 0x0,
			FX_ELEM_RUN_RELATIVE_TO_SPAWN = 0x40,
			FX_ELEM_RUN_RELATIVE_TO_EFFECT = 0x80,
			FX_ELEM_RUN_RELATIVE_TO_OFFSET = 0xC0,
			FX_ELEM_RUN_MASK = 0xC0,
			FX_ELEM_USE_COLLISION = 0x100,
			FX_ELEM_DIE_ON_TOUCH = 0x200,
			FX_ELEM_DRAW_PAST_FOG = 0x400,
			FX_ELEM_DRAW_WITH_VIEWMODEL = 0x800,
			FX_ELEM_BLOCK_SIGHT = 0x1000,
			FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY = 0x2000,
			FX_ELEM_TRAIL_ORIENT_BY_VELOCITY = 0x4000,
			FX_ELEM_EMIT_ORIENT_BY_ELEM = 0x8000,
			FX_ELEM_USE_OCCLUSION_QUERY = 0x10000,
			FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL = 0x1000000,
			FX_ELEM_HAS_VELOCITY_GRAPH_WORLD = 0x2000000,
			FX_ELEM_HAS_GRAVITY = 0x4000000,
			FX_ELEM_USE_MODEL_PHYSICS = 0x8000000,
			FX_ELEM_NONUNIFORM_SCALE = 0x10000000,
			FX_ELEM_CLOUD_SHAPE_CUBE = 0x0,
			FX_ELEM_CLOUD_SHAPE_SPHERE_LARGE = 0x20000000,
			FX_ELEM_CLOUD_SHAPE_SPHERE_MEDIUM = 0x40000000,
			FX_ELEM_CLOUD_SHAPE_SPHERE_SMALL = 0x60000000,
			FX_ELEM_CLOUD_SHAPE_MASK = 0x60000000,
			FX_ELEM_FOUNTAIN_DISABLE_COLLISION = 0x80000000,
		};

		struct FxElemDef // 0xFC
		{
			unsigned int flags;
			FxSpawnDef spawn;
			FxFloatRange spawnRange;
			FxFloatRange fadeInRange;
			FxFloatRange fadeOutRange;
			float spawnFrustumCullRadius;
			FxIntRange spawnDelayMsec;
			FxIntRange lifeSpanMsec;
			FxFloatRange spawnOrigin[3];
			FxFloatRange spawnOffsetRadius;
			FxFloatRange spawnOffsetHeight;
			FxFloatRange spawnAngles[3];
			FxFloatRange angularVelocity[3];
			FxFloatRange initialRotation;
			FxFloatRange gravity;
			FxFloatRange reflectionFactor;
			FxElemAtlas atlas;
			FxElemType elemType;
			char visualCount;
			char velIntervalCount;
			char visStateIntervalCount;
			FxElemVelStateSample* velSamples; // count = velIntervalCount
			FxElemVisStateSample* visSamples; // count = visStateIntervalCount
			FxElemDefVisuals visuals;
			//If elemType is 0xB, then use markVisuals
			//If elemType is not 0xB and visualCount == 1, then use visual
			//If elemType is not 0xB and visualCount != 1, then use visualsArray
			float collMins[3];
			float collMaxs[3];
			FxEffectDefRef effectOnImpact;
			FxEffectDefRef effectOnDeath;
			FxEffectDefRef effectEmitted;
			FxFloatRange emitDist;
			FxFloatRange emitDistVariance;
			FxElemExtendedDefPtr extended;
			//If elemType == 3, then use trailDef
			//If elemType == 6, then use sparkFountain
			//If elemType != 3 && elemType != 6 use unknownDef (size = 1)
			char sortOrder;
			char lightingFrac;
			char useItemClip;
			char fadeInfo;
			int pad; // IW5 only
		};

		struct FxEffectDef
		{
			const char* name;
			int flags;
			int totalSize;
			int msecLoopingLife;
			int elemDefCountLooping;
			int elemDefCountOneShot;
			int elemDefCountEmission;
			float occlusionQueryDepthBias;
			int occlusionQueryFadeIn;
			int occlusionQueryFadeOut;
			FxFloatRange occlusionQueryScaleRange;
			FxElemDef* elemDefs; //Count = elemDefCountOneShot + elemDefCountEmission + elemDefCountLooping
		};

		// ClipMap
#pragma pack(push, 4)
		struct cStaticModel_s
		{
			XModel* xmodel;
			float origin[3];
			float invScaledAxis[3][3];
			float absmin[3];
			float absmax[3];
		};

		enum CSurfaceFlags : std::uint32_t
		{
			SURF_FLAG_DEFAULT = 0x00000000,
			SURF_FLAG_BARK = 0x00100000,
			SURF_FLAG_BRICK = 0x00200000,
			SURF_FLAG_CARPET = 0x00300000,
			SURF_FLAG_CLOTH = 0x00400000,
			SURF_FLAG_CONCRETE = 0x00500000,
			SURF_FLAG_DIRT = 0x00600000,
			SURF_FLAG_FLESH = 0x00700000,
			SURF_FLAG_FOLIAGE = 0x00800000,
			SURF_FLAG_GLASS = 0x00900000,
			SURF_FLAG_GRASS = 0x00A00000,
			SURF_FLAG_GRAVEL = 0x00B00000,
			SURF_FLAG_ICE = 0x00C00000,
			SURF_FLAG_METAL = 0x00D00000,
			SURF_FLAG_MUD = 0x00E00000,
			SURF_FLAG_PAPER = 0x00F00000,
			SURF_FLAG_PLASTER = 0x01000000,
			SURF_FLAG_ROCK = 0x01100000,
			SURF_FLAG_SAND = 0x01200000,
			SURF_FLAG_SNOW = 0x01300000,
			SURF_FLAG_WATER = 0x01400000,
			SURF_FLAG_WOOD = 0x01500000,
			SURF_FLAG_ASPHALT = 0x01600000,
			SURF_FLAG_CERAMIC = 0x01700000,
			SURF_FLAG_PLASTIC = 0x01800000,
			SURF_FLAG_RUBBER = 0x01900000,
			SURF_FLAG_CUSHION = 0x01A00000,
			SURF_FLAG_FRUIT = 0x01B00000,
			SURF_FLAG_PAINTEDMETAL = 0x01C00000,
			SURF_FLAG_RIOTSHIELD = 0x01D00000,
			SURF_FLAG_SLUSH = 0x01E00000,
			SURF_FLAG_OPAQUEGLASS = 0x00900000,
			SURF_FLAG_CLIPMISSILE = 0x00000000,
			SURF_FLAG_AI_NOSIGHT = 0x00000000,
			SURF_FLAG_CLIPSHOT = 0x00000000,
			SURF_FLAG_PLAYERCLIP = 0x00000000,
			SURF_FLAG_MONSTERCLIP = 0x00000000,
			SURF_FLAG_AICLIPALLOWDEATH = 0x00000000,
			SURF_FLAG_VEHICLECLIP = 0x00000000,
			SURF_FLAG_ITEMCLIP = 0x00000000,
			SURF_FLAG_NODROP = 0x00000000,
			SURF_FLAG_NONSOLID = 0x00004000,
			SURF_FLAG_DETAIL = 0x00000000,
			SURF_FLAG_STRUCTURAL = 0x00000000,
			SURF_FLAG_PORTAL = 0x80000000,
			SURF_FLAG_CANSHOOTCLIP = 0x00000000,
			SURF_FLAG_ORIGIN = 0x00000000,
			SURF_FLAG_SKY = 0x00000004,
			SURF_FLAG_NOCASTSHADOW = 0x00040000,
			SURF_FLAG_PHYSICSGEOM = 0x00000000,
			SURF_FLAG_LIGHTPORTAL = 0x00000000,
			SURF_FLAG_OUTDOORBOUNDS = 0x00000000,
			SURF_FLAG_SLICK = 0x00000002,
			SURF_FLAG_NOIMPACT = 0x00000010,
			SURF_FLAG_NOMARKS = 0x00000020,
			SURF_FLAG_NOPENETRATE = 0x00000100,
			SURF_FLAG_LADDER = 0x00000008,
			SURF_FLAG_NODAMAGE = 0x00000001,
			SURF_FLAG_MANTLEON = 0x02000000,
			SURF_FLAG_MANTLEOVER = 0x04000000,
			SURF_FLAG_STAIRS = 0x00000200,
			SURF_FLAG_SOFT = 0x00001000,
			SURF_FLAG_NOSTEPS = 0x00002000,
			SURF_FLAG_NODRAW = 0x00000080,
			SURF_FLAG_NOLIGHTMAP = 0x00000400,
			SURF_FLAG_NODLIGHT = 0x00020000,
		};

		struct dmaterial_t
		{
			char* material;
			int surfaceFlags;
			int contentFlags;
		};

		struct cNode_t
		{
			cplane_s* plane;
			__int16 children[2];
		};

		struct cLeaf_t
		{
			unsigned __int16 firstCollAabbIndex;
			unsigned __int16 collAabbCount;
			int brushContents;
			int terrainContents;
			Bounds bounds;
			int leafBrushNode;
		};

		struct CollisionBorder
		{
			float distEq[3];
			float zBase;
			float zSlope;
			float start;
			float length;
		};

		struct cLeafBrushNodeLeaf_t
		{
			unsigned __int16* brushes;
		};

		struct cLeafBrushNodeChildren_t
		{
			float dist;
			float range;
			unsigned __int16 childOffset[2];
		};

		union cLeafBrushNodeData_t
		{
			cLeafBrushNodeLeaf_t leaf;
			cLeafBrushNodeChildren_t children;
		};

		struct cLeafBrushNode_s
		{
			char axis;
			__int16 leafBrushCount;
			int contents;
			cLeafBrushNodeData_t data;
		};

		struct CollisionPartition
		{
			char triCount;
			char borderCount;
			char firstVertSegment;
			int firstTri;
			CollisionBorder* borders;
		};

		union CollisionAabbTreeIndex
		{
			int firstChildIndex;
			int partitionIndex;
		};

		struct CollisionAabbTree
		{
			float midPoint[3];
			unsigned __int16 materialIndex;
			unsigned __int16 childCount;
			float halfSize[3];
			CollisionAabbTreeIndex u;
		};

		typedef char cbrushedge_t;

		struct cbrush_t
		{
			unsigned __int16 numsides;
			unsigned __int16 glassPieceIndex;
			cbrushside_t* sides;
			cbrushedge_t* edge;
			__int16 axialMaterialNum[2][3];
			char firstAdjacentSideOffsets[2][3];
			char edgeCount[2][3];
		};

		enum DynEntityType
		{
			DYNENT_TYPE_INVALID = 0x0,
			DYNENT_TYPE_CLUTTER = 0x1,
			DYNENT_TYPE_DESTRUCT = 0x2,
			DYNENT_TYPE_COUNT = 0x3,
		};

		struct GfxPlacement
		{
			float quat[4];
			float origin[3];
		};

		struct PhysMass
		{
			float centerOfMass[3];
			float momentsOfInertia[3];
			float productsOfInertia[3];
			// int contents;
		};

		struct DynEntityHingeDef
		{
			float axisOrigin[3];
			float axisDir[3];
			bool isLimited;
			float angleMin;
			float angleMax;
			float momentOfInertia;
			float friction;
		};

		struct DynEntityDef_IW4
		{
			union
			{
				struct
				{
					DynEntityType type;
					GfxPlacement pose;
					XModel* xModel;
					unsigned __int16 brushModel;
					unsigned __int16 physicsBrushModel;
					FxEffectDef* destroyFx;
					PhysPreset* physPreset;
					int health;
				};

				char _portpad[52];
			};

			PhysMass mass;
		};

		struct DynEntityDef
		{
			union
			{
				struct
				{
					DynEntityType type;
					GfxPlacement pose;
					XModel* xModel;
					unsigned __int16 brushModel;
					unsigned __int16 physicsBrushModel;
					FxEffectDef* destroyFx;
					PhysPreset* physPreset;
					int health;
				};

				char _portpad[52];
			};

			DynEntityHingeDef* hinge;
			PhysMass mass;
			int contents;
		};

		struct DynEntityPose
		{
			GfxPlacement pose;
			float radius;
		};

		struct DynEntityClient
		{
			int physObjId;
			unsigned __int16 flags;
			unsigned __int16 lightingHandle;
			int health;
			int contents;
		};

		struct DynEntityColl
		{
			unsigned __int16 sector;
			unsigned __int16 nextEntInSector;
			float linkMins[2];
			float linkMaxs[2];
		};

		struct Stage
		{
			char* name;
			float origin[3];
			unsigned __int16 triggerIndex;
			char sunPrimaryLightIndex;
		};

		struct SModelAabbNode
		{
			Bounds bounds;
			short firstChild;
			short childCount;
		};

		struct ClipInfo
		{
			int numCPlanes;
			cplane_s* cPlanes;
			int numMaterials;
			dmaterial_t* materials;
			int numCBrushSides;
			cbrushside_t* cBrushSides;
			int numCBrushEdges;
			cbrushedge_t* cBrushEdges;
			int numCLeafBrushNodes;
			cLeafBrushNode_s* cLeafBrushNodes; // cmodels use this?
			int numLeafBrushes;
			short* leafBrushes;
			short numBrushes;
			cbrush_t* brushes;
			Bounds* brushBounds;
			int* brushContents;
		};

		struct cmodel_t
		{
			union
			{
				char _portpad0[28];

				struct
				{
					Bounds bounds;
					float radius;
				};
			};

			ClipInfo* info;

			union
			{
				char _portpad1[40];

				struct
				{
					cLeaf_t leaf;
				};
			};
		};

		struct cmodel_t_IW4
		{
			union
			{
				char _portpad0[28];

				struct
				{
					Bounds bounds;
					float radius;
				};
			};

			union
			{
				char _portpad1[40];

				struct
				{
					cLeaf_t leaf;
				};
			};
		};

		struct clipMap_t
		{
			const char* name;
			bool isInUse;
			char pad1[3];
			ClipInfo info;
			ClipInfo* pInfo;
			int numStaticModels;
			cStaticModel_s* staticModelList;
			int numCNodes;
			cNode_t* cNodes;
			int numCLeaf;
			cLeaf_t* cLeaf;
			int numVerts;
			VecInternal<3>* verts;
			int numTriIndices;
			short* triIndices;
			char* triEdgeIsWalkable; //Size = ((triCount << 1) + triCount + 0x1F) >> 3 << 2
			int numCollisionBorders;
			CollisionBorder* collisionBorders;
			int numCollisionPartitions;
			CollisionPartition* collisionPartitions;
			int numCollisionAABBTrees;
			CollisionAabbTree* collisionAABBTrees;
			int numCModels;
			cmodel_t* cModels;
			MapEnts* mapEnts;
			Stage* stages;
			unsigned char stageCount;
			char pad2[3];
			MapTriggers trigger;
			short smodelNodeCount;
			SModelAabbNode* smodelNodes;
			unsigned __int16 dynEntCount[2];
			DynEntityDef* dynEntDefList[2];
			DynEntityPose* dynEntPoseList[2];
			DynEntityClient* dynEntClientList[2];
			DynEntityColl* dynEntCollList[2];
			char pad3[20];
			std::uint32_t isPlutoniumMap;
		};
#pragma pack(pop)

#pragma pack(push, 1)
		struct ComPrimaryLight
		{
			union
			{
				char _portpad0[28];

				struct
				{
					char type;
					char canUseShadowMap;
					char exponent;
					char unused;
					float color[3];
					float dir[3];
				};
			};

			float up[3];

			union
			{
				char _portpad1[40];

				struct
				{
					float origin[3];
					float radius;
					float cosHalfFovOuter;
					float cosHalfFovInner;
					float cosHalfFovExpanded;
					float rotationLimit;
					float translationLimit;
					const char* defName;
				};
			};
		};
#pragma pack(pop)

		struct ComWorld
		{
			const char* name;
			int isInUse;
			unsigned int primaryLightCount;
			ComPrimaryLight* primaryLights;
		};

#pragma pack(push, 4)
		struct GfxLightImage
		{
			GfxImage* image;
			char samplerState;
		};
#pragma pack(pop)

		struct GfxLightDef
		{
			const char* name;
			GfxLightImage attenuation;
			GfxLightImage cucoloris;
			int lmapLookupStart;
		};

		struct AddonMapEnts
		{
			const char* name;
			char* entityString;
			int numEntityChars;
			MapTriggers trigger; // See the MapEnts Asset
			ClipInfo* info; // See the Collision Map Asset
			unsigned int numSubModels;
			void* cmodels; // See the Collision Map Asset
			void* models; // See the Graphics Map Asset
		};

		struct G_GlassName
		{
			char* nameStr;
			unsigned __int16 name;
			unsigned __int16 pieceCount;
			unsigned __int16* pieceIndices;
		};

#pragma pack(push, 2)
		struct G_GlassPiece
		{
			unsigned __int16 damageTaken;
			unsigned __int16 collapseTime;
			int lastStateChangeTime;
			char impactDir;
			char impactPos[2];
		};
#pragma pack(pop)

		struct G_GlassData
		{
			G_GlassPiece* glassPieces;
			unsigned int pieceCount;
			unsigned __int16 damageToWeaken;
			unsigned __int16 damageToDestroy;
			unsigned int glassNameCount;
			G_GlassName* glassNames;
			char pad[108];
		};

		struct GlassWorld
		{
			const char* name;
			G_GlassData* g_glassData;
		};

		struct ScriptFile
		{
			const char* name;
			unsigned int compressedLen;
			unsigned int len;
			unsigned int bytecodeLen;
			const char* buffer;
			char* bytecode;
		};

#pragma pack(push, 2)
		struct Glyph
		{
			unsigned __int16 letter;
			char x0;
			char y0;
			char dx;
			char pixelWidth;
			char pixelHeight;
			float s0;
			float t0;
			float s1;
			float t1;
		};
#pragma pack(pop)

		struct Font_s
		{
			const char* name; // 0
			int pixelHeight; // 4
			int glyphCount; // 8
			Material* material; // 12
			Material* glowMaterial; // 16
			Glyph* glyphs; // 20
		};

		// GfxMap
#pragma pack(push, 4)
		struct GfxSky
		{
			std::uint32_t skySurfCount;
			std::uint32_t* skyStartSurfs;
			GfxImage* skyImage;
			char skySamplerState;
			char pad[3];
		};

		struct GfxWorldDpvsPlanes
		{
			int cellCount;
			cplane_s* planes;
			unsigned __int16* nodes;
			unsigned char* sceneEntCellBits; //Size = cellCount << 11
		};

		struct GfxCellTreeCount
		{
			int aabbTreeCount;
		};

		struct GfxAabbTree
		{
			float mins[3];
			float maxs[3];
			unsigned __int16 childCount;
			unsigned __int16 surfaceCount;
			unsigned __int16 startSurfIndex;
			unsigned __int16 surfaceCountNoDecal;
			unsigned __int16 startSurfIndexNoDecal;
			unsigned __int16 smodelIndexCount;
			unsigned __int16* smodelIndexes;
			int childrenOffset;
		};
		// Size: 0x2C
		static_assert(sizeof(GfxAabbTree) == 0x2C);
		static_assert(offsetof(GfxAabbTree, childCount) == 24);
		static_assert(offsetof(GfxAabbTree, smodelIndexCount) == 34);
		static_assert(offsetof(GfxAabbTree, smodelIndexes) == 36);
		static_assert(offsetof(GfxAabbTree, childrenOffset) == 40);

		struct GfxCellTree
		{
			// Best struct ever
			GfxAabbTree* aabbtree;
		};

		struct GfxPortal;
		struct GfxPortalWritable
		{
			bool isQueued;
			bool isAncestor;
			char recursionDepth;
			char hullPointCount;
			float(*hullPoints)[2];
			GfxPortal* queuedParent;
		};

		struct DpvsPlane
		{
			float coeffs[4];
		};

		struct GfxPortal
		{
			GfxPortalWritable writable;
			DpvsPlane plane;
			float(*vertices)[3];
			unsigned short cellIndex;
			char vertexCount;
			float hullAxis[2][3];
		};

		struct GfxCell_IW4
		{
			float mins[3];
			float maxs[3];
			int portalCount;
			GfxPortal* portals;
			char reflectionProbeCount;
			char* reflectionProbes;
		};

		struct GfxCell
		{
			float mins[3];
			float maxs[3];
			int portalCount;
			GfxPortal* portals;
			char reflectionProbeCount;
			char* reflectionProbes;
			char reflectionProbeReferenceCount;
			char* reflectionProbeReferences;
		};

		struct GfxReflectionProbe
		{
			float offset[3];
		};

		struct GfxTexture
		{
			GfxImageLoadDef* loadDef;
		};

		struct GfxLightmapArray
		{
			GfxImage* primary;
			GfxImage* secondary;
		};

		struct GfxWorldVertex
		{
			float xyz[3];
			float binormalSign;
			GfxColor color;
			float texCoord[2];
			float lmapCoord[2];
			PackedUnitVec normal;
			PackedUnitVec tangent;
		};

		struct GfxWorldVertexData
		{
			GfxWorldVertex* vertices;
			void* worldVb; // D3DVertexBuffer
		};

		struct GfxWorldVertexLayerData
		{
			char* data;
			void* layerVb; // D3DVertexBuffer
		};

		struct GfxReflectionProbeReferenceOrigin
		{
			float origin[3];
		};

		struct GfxWorldDraw_IW4
		{
			union
			{
				char _portpad0[16];

				struct
				{
					unsigned int reflectionProbeCount; // 4
					GfxImage* * reflectionImages; // 4
					GfxReflectionProbe* reflectionProbes; // 4
					GfxTexture* reflectionProbeTextures; //Count = reflectionProbeCount // 4
				};
			};

			union
			{
				char _portpad1[56];

				struct
				{
					int lightmapCount; // 4
					GfxLightmapArray* lightmaps; // 4
					GfxTexture* lightmapPrimaryTextures; //Count = lightmapCount // 4
					GfxTexture* lightmapSecondaryTextures; //Count = lightmapCount // 4
					GfxImage* skyImage; // 4
					GfxImage* outdoorImage; // 4
					unsigned int vertexCount; // 4
					GfxWorldVertexData vd;
					unsigned int vertexLayerDataSize;
					GfxWorldVertexLayerData vld;
					int indexCount;
					unsigned __int16* indices;
				};
			};
		};

		struct GfxWorldDraw
		{
			union
			{
				char _portpad0[16];

				struct
				{
					unsigned int reflectionProbeCount; // 4
					GfxImage* * reflectionImages; // 4
					GfxReflectionProbe* reflectionProbes; // 4
					GfxTexture* reflectionProbeTextures; //Count = reflectionProbeCount // 4
				};
			};

			int reflectionProbeReferenceCount;
			GfxReflectionProbeReferenceOrigin* reflectionProbeReferenceOrigins;
			char* reflectionProbeReferences;

			union
			{
				char _portpad1[56];

				struct
				{
					int lightmapCount; // 4
					GfxLightmapArray* lightmaps; // 4
					GfxTexture* lightmapPrimaryTextures; //Count = lightmapCount // 4
					GfxTexture* lightmapSecondaryTextures; //Count = lightmapCount // 4
					GfxImage* skyImage; // 4
					GfxImage* outdoorImage; // 4
					unsigned int vertexCount; // 4
					GfxWorldVertexData vd;
					unsigned int vertexLayerDataSize;
					GfxWorldVertexLayerData vld;
					int indexCount;
					unsigned __int16* indices;
				};
			};
		};

		struct GfxLightGridEntry
		{
			unsigned __int16 colorsIndex;
			char primaryLightIndex;
			char needsTrace;
		};

		struct GfxLightGridColors
		{
			char rgb[56][3];
		};

		struct GfxLightGrid
		{
			bool hasLightRegions; // 4
			unsigned int sunPrimaryLightIndex; // 4
			unsigned __int16 mins[3]; // 6
			unsigned __int16 maxs[3]; // 6
			unsigned int rowAxis; // 4
			unsigned int colAxis; // 4
			unsigned __int16* rowDataStart;
			// Size: (varGfxLightGrid->maxs[varGfxLightGrid->rowAxis] - varGfxLightGrid->mins[varGfxLightGrid->rowAxis] + 1) * 2
			unsigned int rawRowDataSize;
			char* rawRowData;
			unsigned int entryCount;
			GfxLightGridEntry* entries;
			unsigned int colorCount;
			GfxLightGridColors* colors;
		};

		struct GfxBrushModelWritable
		{
			Bounds bounds;
		};

		struct GfxBrushModel
		{
			GfxBrushModelWritable writable;
			Bounds bounds;
			float radius;
			unsigned short surfaceCount;
			unsigned short startSurfIndex;
			unsigned short surfaceCountNoDecal;
		};

		struct MaterialMemory
		{
			Material* material;
			std::uint32_t memory;
		};

		struct sunflare_t
		{
			bool hasValidData;
			Material* spriteMaterial;
			Material* flareMaterial;
			float spriteSize;
			float flareMinSize;
			float flareMinDot;
			float flareMaxSize;
			float flareMaxDot;
			float flareMaxAlpha;
			int flareFadeInTime;
			int flareFadeOutTime;
			float blindMinDot;
			float blindMaxDot;
			float blindMaxDarken;
			int blindFadeInTime;
			int blindFadeOutTime;
			float glareMinDot;
			float glareMaxDot;
			float glareMaxLighten;
			int glareFadeInTime;
			int glareFadeOutTime;
			float sunFxPosition[3];
		};

		struct XModelDrawInfo
		{
			char hasGfxEntIndex;
			char lod;
			unsigned __int16 surfId;
		};

		struct GfxSceneDynModel
		{
			XModelDrawInfo info;
			unsigned __int16 dynEntId;
		};

		struct BModelDrawInfo
		{
			unsigned __int16 surfId;
		};

		struct GfxSceneDynBrush
		{
			BModelDrawInfo info;
			unsigned __int16 dynEntId;
		};

		struct GfxShadowGeometry
		{
			unsigned __int16 surfaceCount;
			unsigned __int16 smodelCount;
			unsigned __int16* sortedSurfIndex;
			unsigned __int16* smodelIndex;
		};

		struct GfxLightRegionAxis
		{
			float dir[3];
			float midPoint;
			float halfSize;
		};

		struct GfxLightRegionHull
		{
			float kdopMidPoint[9];
			float kdopHalfSize[9];
			unsigned int axisCount;
			GfxLightRegionAxis* axis;
		};

		struct GfxLightRegion
		{
			unsigned int hullCount;
			GfxLightRegionHull* hulls;
		};

		struct GfxStaticModelInst
		{
			float mins[3];
			float maxs[3];
			float lightingOrigin[3];
		};

		struct srfTriangles_t
		{
			int vertexLayerData;
			int firstVertex;
			unsigned __int16 vertexCount;
			unsigned __int16 triCount;
			int baseIndex;
		};

		struct GfxSurface
		{
			srfTriangles_t tris;
			Material* material;
			char lightmapIndex;
			char reflectionProbeIndex;
			char primaryLightIndex;
			char flags;
		};

		struct GfxSurfaceBounds
		{
			Bounds bounds;
		};

		struct GfxDrawSurfFields
		{
			unsigned __int64 objectId : 15;
			unsigned __int64 reflectionProbeIndex : 8;
			unsigned __int64 hasGfxEntIndex : 1;
			unsigned __int64 customIndex : 5;
			unsigned __int64 materialSortedIndex : 12;
			unsigned __int64 prepass : 2;
			unsigned __int64 useHeroLighting : 1;
			unsigned __int64 sceneLightIndex : 8;
			unsigned __int64 viewModelRender : 1;
			unsigned __int64 surfType : 4;
			unsigned __int64 primarySortKey : 6;
			unsigned __int64 unused : 1;
		};

		union GfxDrawSurf
		{
			GfxDrawSurfFields fields;
			unsigned __int64 packed;
		};

		struct GfxPackedPlacement
		{
			float origin[3];
			float axis[3][3];
			float scale;
		};

		struct GfxStaticModelDrawInst
		{
			GfxPackedPlacement placement;
			XModel* model;
			unsigned __int16 cullDist;
			unsigned __int16 lightingHandle;
			char reflectionProbeIndex;
			char primaryLightIndex;
			char flags;
			char firstMtlSkinIndex;
			GfxColor groundLighting;
			unsigned __int16 cacheId[4];
		};

		struct GfxWorldDpvsDynamic
		{
			unsigned int dynEntClientWordCount[2];
			unsigned int dynEntClientCount[2];
			unsigned int* dynEntCellBits[2];
			char* dynEntVisData[2][3];
		};

		struct GfxWorldDpvsStatic
		{
			unsigned int smodelCount;
			unsigned int staticSurfaceCount;
			unsigned int staticSurfaceCountNoDecal;
			unsigned int litOpaqueSurfsBegin;
			unsigned int litOpaqueSurfsEnd;
			unsigned int litTransSurfsBegin;
			unsigned int litTransSurfsEnd;
			unsigned int shadowCasterSurfsBegin;
			unsigned int shadowCasterSurfsEnd;
			unsigned int emissiveSurfsBegin;
			unsigned int emissiveSurfsEnd;
			unsigned int smodelVisDataCount;
			unsigned int surfaceVisDataCount;
			char* smodelVisData[3];
			char* surfaceVisData[3];
			unsigned __int16* sortedSurfIndex;
			GfxStaticModelInst* smodelInsts;
			GfxSurface* surfaces;
			GfxSurfaceBounds* surfacesBounds;
			GfxStaticModelDrawInst* smodelDrawInsts;
			GfxDrawSurf* surfaceMaterials;
			unsigned int* surfaceCastsSunShadow;
			volatile int usageCount;
		};

		struct GfxHeroLight
		{
			char type;
			char pad[3];
			float color[3];
			float dir[3];
			float up[3];
			float origin[3];
			float radius;
			float cosHalfFovOuter;
			float cosHalfFovInner;
			int exponent;
		};

		struct GfxWorld
		{
			const char* name; // 4
			const char* baseName; // 4
			int planeCount; // 4
			int nodeCount; // 4 // = 16
			std::uint32_t surfaceCount; // 4
			int skyCount; // 4
			GfxSky* skies; // 4
			std::uint32_t lastSunPrimaryLightIndex;
			std::uint32_t primaryLightCount;
			std::uint32_t sortKeyLitDecal;
			std::uint32_t sortKeyEffectDecal;
			std::uint32_t sortKeyEffectAuto;
			std::uint32_t sortKeyDistortion;
			GfxWorldDpvsPlanes dpvsPlanes; // 16
			GfxCellTreeCount* aabbTreeCounts; // Size: 4 * dpvsPlanes.cellCount // 4
			GfxCellTree* aabbTree; // 4
			GfxCell* cells; // 4  // = 80
			GfxWorldDraw worldDraw; // 72
			GfxLightGrid lightGrid; // 56 // = 208
			int modelCount; // 4
			GfxBrushModel* models; // 4 // = 216
			Bounds bounds;
			std::uint32_t checksum; // 4
			int materialMemoryCount; // 4 // = 248
			MaterialMemory* materialMemory; // 4
			sunflare_t sun; // 96 // = 348
			float outdoorLookupMatrix[4][4]; // 64
			GfxImage* outdoorImage; // 4 // = 416
			std::uint32_t* cellCasterBits[2]; // 8
			GfxSceneDynModel* sceneDynModel; // 4
			GfxSceneDynBrush* sceneDynBrush; // 4 // = 432
			std::uint32_t* primaryLightEntityShadowVis;
			std::uint32_t* primaryLightDynEntShadowVis[2];
			unsigned char* primaryLightForModelDynEnt;
			GfxShadowGeometry* shadowGeom;
			GfxLightRegion* lightRegion;
			GfxWorldDpvsStatic dpvs;
			GfxWorldDpvsDynamic dpvsDyn;
			std::uint32_t mapVtxChecksum;
			std::uint32_t heroLightCount;
			GfxHeroLight* heroLights;
			unsigned char fogTypesAllowed;
		};
#pragma pack(pop)

		enum weapOverlayReticle_t : int
		{
			WEAPOVERLAYRETICLE_NONE = 0x0,
			WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,
			WEAPOVERLAYRETICLE_NUM = 0x2
		};

		enum weapOverlayInterface_t : int
		{
			WEAPOVERLAYINTERFACE_NONE = 0x0,
			WEAPOVERLAYINTERFACE_JAVELIN = 0x1,
			WEAPOVERLAYINTERFACE_TURRETSCOPE = 0x2,
			WEAPOVERLAYINTERFACECOUNT = 0x3
		};

		enum activeReticleType_t : int
		{
			VEH_ACTIVE_RETICLE_NONE = 0,
			VEH_ACTIVE_RETICLE_PIP_ON_A_STICK = 1,
			VEH_ACTIVE_RETICLE_BOUNCING_DIAMOND = 2,
			VEH_ACTIVE_RETICLE_MAX
		};

		enum WeapStickinessType : int
		{
			WEAPSTICKINESS_NONE = 0x0,
			WEAPSTICKINESS_ALL = 0x1,
			WEAPSTICKINESS_ALL_ORIENT = 0x2,
			WEAPSTICKINESS_GROUND = 0x3,
			WEAPSTICKINESS_GROUND_WITH_YAW = 0x4,
			WEAPSTICKINESS_KNIFE = 0x5,
			WEAPSTICKINESS_COUNT = 0x6
		};

		enum guidedMissileType_t : int
		{
			MISSILE_GUIDANCE_NONE = 0x0,
			MISSILE_GUIDANCE_SIDEWINDER = 0x1,
			MISSILE_GUIDANCE_HELLFIRE = 0x2,
			MISSILE_GUIDANCE_JAVELIN = 0x3,
			MISSILE_GUIDANCE_MAX
		};

		union snd_alias_list_name
		{
			const char* name;
			snd_alias_list_t* asset;
		};

		struct ADSOverlay
		{
			Material* shader;
			Material* shaderLowRes;
			Material* shaderEMP;
			Material* shaderEMPLowRes;
			weapOverlayReticle_t reticle;
			float width;
			float height;
			float widthSplitscreen;
			float heightSplitscreen;
		};

		typedef BYTE _BYTE;

#pragma pack(push, 4)
		struct StateTimers
		{
			int iFireDelay;
			int iMeleeDelay;
			int meleeChargeDelay;
			int iDetonateDelay;
			int iRechamberTime;
			int rechamberTimeOneHanded;
			int iRechamberBoltTime;
			int iHoldFireTime;
			int iDetonateTime;
			int iMeleeTime;
			int meleeChargeTime;
			int iReloadTime;
			int reloadShowRocketTime;
			int iReloadEmptyTime;
			int iReloadAddTime;
			int iReloadStartTime;
			int iReloadStartAddTime;
			int iReloadEndTime;
			int iDropTime;
			int iRaiseTime;
			int iAltDropTime;
			int quickDropTime;
			int quickRaiseTime;
			int iBreachRaiseTime;
			int iEmptyRaiseTime;
			int iEmptyDropTime;
			int sprintInTime;
			int sprintLoopTime;
			int sprintOutTime;
			int stunnedTimeBegin;
			int stunnedTimeLoop;
			int stunnedTimeEnd;
			int nightVisionWearTime;
			int nightVisionWearTimeFadeOutEnd;
			int nightVisionWearTimePowerUp;
			int nightVisionRemoveTime;
			int nightVisionRemoveTimePowerDown;
			int nightVisionRemoveTimeFadeInStart;
			int fuseTime;
			int aiFuseTime;
			int blastFrontTime;
			int blastRightTime;
			int blastBackTime;
			int blastLeftTime;
			int raiseInterruptableTime;
			int firstRaiseInterruptableTime;
			int reloadInterruptableTime;
			int reloadEmptyInterruptableTime;
			int fireInterruptableTime;
		};
		struct WeaponDef
		{
			const char* szOverlayName;
			XModel** gunXModel;
			XModel* handXModel;
			const char** szXAnimsRightHanded;
			const char** szXAnimsLeftHanded;
			const char* szModeName;
			__int16* notetrackSoundMapKeys;
			__int16* notetrackSoundMapValues;
			__int16* notetrackRumbleMapKeys;
			__int16* notetrackRumbleMapValues;
			int playerAnimType;
			int weapType;
			int weapClass;
			int penetrateType;
			int inventoryType;
			int fireType;
			int offhandClass;
			int stance;
			FxEffectDef* viewFlashEffect;
			FxEffectDef* worldFlashEffect;
			union
			{
				struct
				{
					snd_alias_list_t* pickupSound;
					snd_alias_list_t* pickupSoundPlayer;
					snd_alias_list_t* ammoPickupSound;
					snd_alias_list_t* ammoPickupSoundPlayer;
					snd_alias_list_t* projectileSound;
					snd_alias_list_t* pullbackSound;
					snd_alias_list_t* pullbackSoundPlayer;
					snd_alias_list_t* fireSound;
					snd_alias_list_t* fireSoundPlayer;
					snd_alias_list_t* fireSoundPlayerAkimbo;
					snd_alias_list_t* fireLoopSound;
					snd_alias_list_t* fireLoopSoundPlayer;
					snd_alias_list_t* fireStopSound;
					snd_alias_list_t* fireStopSoundPlayer;
					snd_alias_list_t* fireLastSound;
					snd_alias_list_t* fireLastSoundPlayer;
					snd_alias_list_t* emptyFireSound;
					snd_alias_list_t* emptyFireSoundPlayer;
					snd_alias_list_t* meleeSwipeSound;
					snd_alias_list_t* meleeSwipeSoundPlayer;
					snd_alias_list_t* meleeHitSound;
					snd_alias_list_t* meleeMissSound;
					snd_alias_list_t* rechamberSound;
					snd_alias_list_t* rechamberSoundPlayer;
					snd_alias_list_t* reloadSound;
					snd_alias_list_t* reloadSoundPlayer;
					snd_alias_list_t* reloadEmptySound;
					snd_alias_list_t* reloadEmptySoundPlayer;
					snd_alias_list_t* reloadStartSound;
					snd_alias_list_t* reloadStartSoundPlayer;
					snd_alias_list_t* reloadEndSound;
					snd_alias_list_t* reloadEndSoundPlayer;
					snd_alias_list_t* detonateSound;
					snd_alias_list_t* detonateSoundPlayer;
					snd_alias_list_t* nightVisionWearSound;
					snd_alias_list_t* nightVisionWearSoundPlayer;
					snd_alias_list_t* nightVisionRemoveSound;
					snd_alias_list_t* nightVisionRemoveSoundPlayer;
					snd_alias_list_t* altSwitchSound;
					snd_alias_list_t* altSwitchSoundPlayer;
					snd_alias_list_t* raiseSound;
					snd_alias_list_t* raiseSoundPlayer;
					snd_alias_list_t* firstRaiseSound;
					snd_alias_list_t* firstRaiseSoundPlayer;
					snd_alias_list_t* putawaySound;
					snd_alias_list_t* putawaySoundPlayer;
					snd_alias_list_t* scanSound;
					snd_alias_list_t* changeVariableZoomSound;
				};
				snd_alias_list_t* sounds[48];
			};
			snd_alias_list_t** bounceSound;
			snd_alias_list_t** rollingSound;
			FxEffectDef* viewShellEjectEffect;
			FxEffectDef* worldShellEjectEffect;
			FxEffectDef* viewLastShotEjectEffect;
			FxEffectDef* worldLastShotEjectEffect;
			Material* reticleCenter;
			Material* reticleSide;
			int iReticleCenterSize;
			int iReticleSideSize;
			int iReticleMinOfs;
			activeReticleType_t activeReticleType;
			float vStandMove[3];
			float vStandRot[3];
			float strafeMove[3];
			float strafeRot[3];
			float vDuckedOfs[3];
			float vDuckedMove[3];
			float vDuckedRot[3];
			float vProneOfs[3];
			float vProneMove[3];
			float vProneRot[3];
			float fPosMoveRate;
			float fPosProneMoveRate;
			float fStandMoveMinSpeed;
			float fDuckedMoveMinSpeed;
			float fProneMoveMinSpeed;
			float fPosRotRate;
			float fPosProneRotRate;
			float fStandRotMinSpeed;
			float fDuckedRotMinSpeed;
			float fProneRotMinSpeed;
			XModel** worldModel;
			XModel* worldClipModel;
			XModel* rocketModel;
			XModel* knifeModel;
			XModel* worldKnifeModel;
			Material* hudIcon;
			weaponIconRatioType_t hudIconRatio;
			Material* pickupIcon;
			weaponIconRatioType_t pickupIconRatio;
			Material* ammoCounterIcon;
			weaponIconRatioType_t ammoCounterIconRatio;
			ammoCounterClipType_t ammoCounterClip;
			int iStartAmmo;
			const char* szAmmoName;
			int iAmmoIndex;
			const char* szClipName;
			int iClipIndex;
			int iMaxAmmo;
			int shotCount;
			const char* szSharedAmmoCapName;
			int iSharedAmmoCapIndex;
			int iSharedAmmoCap;
			int damage;
			int playerDamage;
			int iMeleeDamage;
			int iDamageType;
			StateTimers stateTimers;
			StateTimers akimboStateTimers;
			float autoAimRange;
			float aimAssistRange;
			float aimAssistRangeAds;
			float aimPadding;
			float enemyCrosshairRange;
			float moveSpeedScale;
			float adsMoveSpeedScale;
			float sprintDurationScale;
			float fAdsZoomInFrac;
			float fAdsZoomOutFrac;
			ADSOverlay overlay;
			int overlayInterface;
			float fAdsBobFactor;
			float fAdsViewBobMult;
			float fHipSpreadStandMin;
			float fHipSpreadDuckedMin;
			float fHipSpreadProneMin;
			float hipSpreadStandMax;
			float hipSpreadDuckedMax;
			float hipSpreadProneMax;
			float fHipSpreadDecayRate;
			float fHipSpreadFireAdd;
			float fHipSpreadTurnAdd;
			float fHipSpreadMoveAdd;
			float fHipSpreadDuckedDecay;
			float fHipSpreadProneDecay;
			float fHipReticleSidePos;
			float fAdsIdleAmount;
			float fHipIdleAmount;
			float adsIdleSpeed;
			float hipIdleSpeed;
			float fIdleCrouchFactor;
			float fIdleProneFactor;
			float fGunMaxPitch;
			float fGunMaxYaw;
			float adsIdleLerpStartTime;
			float adsIdleLerpTime;
			float swayMaxAngle;
			float swayLerpSpeed;
			float swayPitchScale;
			float swayYawScale;
			float swayHorizScale;
			float swayVertScale;
			float swayShellShockScale;
			float adsSwayMaxAngle;
			float adsSwayLerpSpeed;
			float adsSwayPitchScale;
			float adsSwayYawScale;
			float adsSwayHorizScale;
			float adsSwayVertScale;
			float adsViewErrorMin;
			float adsViewErrorMax;
			PhysCollmap* physCollmap;
			float dualWieldViewModelOffset;
			weaponIconRatioType_t killIconRatio;
			int iReloadAmmoAdd;
			int iReloadStartAdd;
			int ammoDropStockMin;
			int ammoDropClipPercentMin;
			int ammoDropClipPercentMax;
			int iExplosionRadius;
			int iExplosionRadiusMin;
			int iExplosionInnerDamage;
			int iExplosionOuterDamage;
			float damageConeAngle;
			float bulletExplDmgMult;
			float bulletExplRadiusMult;
			int iProjectileSpeed;
			int iProjectileSpeedUp;
			int iProjectileSpeedForward;
			int iProjectileActivateDist;
			float projLifetime;
			float timeToAccelerate;
			float projectileCurvature;
			XModel* projectileModel;
			int projExplosion;
			FxEffectDef* projExplosionEffect;
			FxEffectDef* projDudEffect;
			snd_alias_list_t* projExplosionSound;
			snd_alias_list_t* projDudSound;
			WeapStickinessType stickiness;
			float lowAmmoWarningThreshold;
			float ricochetChance;
			bool riotShieldEnableDamage;
			int riotShieldHealth;
			float riotShieldDamageMult;
			float* parallelBounce;
			float* perpendicularBounce;
			FxEffectDef* projTrailEffect;
			FxEffectDef* projBeaconEffect;
			float vProjectileColor[3];
			guidedMissileType_t guidedMissileType;
			float maxSteeringAccel;
			int projIgnitionDelay;
			FxEffectDef* projIgnitionEffect;
			snd_alias_list_t* projIgnitionSound;
			float fAdsAimPitch;
			float fAdsCrosshairInFrac;
			float fAdsCrosshairOutFrac;
			int adsGunKickReducedKickBullets;
			float adsGunKickReducedKickPercent;
			float fAdsGunKickPitchMin;
			float fAdsGunKickPitchMax;
			float fAdsGunKickYawMin;
			float fAdsGunKickYawMax;
			float fAdsGunKickAccel;
			float fAdsGunKickSpeedMax;
			float fAdsGunKickSpeedDecay;
			float fAdsGunKickStaticDecay;
			float fAdsViewKickPitchMin;
			float fAdsViewKickPitchMax;
			float fAdsViewKickYawMin;
			float fAdsViewKickYawMax;
			float fAdsViewScatterMin;
			float fAdsViewScatterMax;
			float fAdsSpread;
			int hipGunKickReducedKickBullets;
			float hipGunKickReducedKickPercent;
			float fHipGunKickPitchMin;
			float fHipGunKickPitchMax;
			float fHipGunKickYawMin;
			float fHipGunKickYawMax;
			float fHipGunKickAccel;
			float fHipGunKickSpeedMax;
			float fHipGunKickSpeedDecay;
			float fHipGunKickStaticDecay;
			float fHipViewKickPitchMin;
			float fHipViewKickPitchMax;
			float fHipViewKickYawMin;
			float fHipViewKickYawMax;
			float fHipViewScatterMin;
			float fHipViewScatterMax;
			float fightDist;
			float maxDist;
			const char* accuracyGraphName[2];
			vec2_t* accuracyGraphKnots;
			vec2_t* originalAccuracyGraphKnots;
			short accuracyGraphKnotCount;
			short originalAccuracyGraphKnotCount;
			int iPositionReloadTransTime;
			float leftArc;
			float rightArc;
			float topArc;
			float bottomArc;
			float accuracy;
			float aiSpread;
			float playerSpread;
			float minTurnSpeed[2];
			float maxTurnSpeed[2];
			float pitchConvergenceTime;
			float yawConvergenceTime;
			float suppressTime;
			float maxRange;
			float fAnimHorRotateInc;
			float fPlayerPositionDist;
			const char* szUseHintString;
			const char* dropHintString;
			int iUseHintStringIndex;
			int dropHintStringIndex;
			float horizViewJitter;
			float vertViewJitter;
			float scanSpeed;
			float scanAccel;
			int scanPauseTime;
			const char* szScript;
			float fOOPosAnimLength[2];
			int minDamage;
			int minPlayerDamage;
			float fMaxDamageRange;
			float fMinDamageRange;
			float destabilizationRateTime;
			float destabilizationCurvatureMax;
			int destabilizeDistance;
			float* locationDamageMultipliers;
			const char* fireRumble;
			const char* meleeImpactRumble;
			TracerDef* tracerType;
			bool turretADSEnabled;
			float turretADSTime;
			float turretFov;
			float turretFovADS;
			float turretScopeZoomRate;
			float turretScopeZoomMin;
			float turretScopeZoomMax;
			float turretOverheatUpRate;
			float turretOverheatDownRate;
			float turretOverheatPenalty;
			snd_alias_list_t* turretOverheatSound;
			FxEffectDef* turretOverheatEffect;
			const char* turretBarrelSpinRumble;
			float turretBarrelSpinSpeed;
			float turretBarrelSpinUpTime;
			float turretBarrelSpinDownTime;
			snd_alias_list_t* turretBarrelSpinMaxSnd;
			snd_alias_list_t* turretBarrelSpinUpSnd[4];
			snd_alias_list_t* turretBarrelSpinDownSnd[4];
			snd_alias_list_t* missileConeSoundAlias;
			snd_alias_list_t* missileConeSoundAliasAtBase;
			float missileConeSoundRadiusAtTop;
			float missileConeSoundRadiusAtBase;
			float missileConeSoundHeight;
			float missileConeSoundOriginOffset;
			float missileConeSoundVolumescaleAtCore;
			float missileConeSoundVolumescaleAtEdge;
			float missileConeSoundVolumescaleCoreSize;
			float missileConeSoundPitchAtTop;
			float missileConeSoundPitchAtBottom;
			float missileConeSoundPitchTopSize;
			float missileConeSoundPitchBottomSize;
			float missileConeSoundCrossfadeTopSize;
			float missileConeSoundCrossfadeBottomSize;
			bool sharedAmmo;
			bool lockonSupported;
			bool requireLockonToFire;
			bool isAirburstWeapon;
			bool bigExplosion;
			bool noAdsWhenMagEmpty;
			bool avoidDropCleanup;
			bool inheritsPerks;
			bool crosshairColorChange;
			bool bRifleBullet;
			bool armorPiercing;
			bool bBoltAction;
			bool aimDownSight;
			bool canHoldBreath;
			bool canVariableZoom;
			bool bRechamberWhileAds;
			bool bBulletExplosiveDamage;
			bool bCookOffHold;
			bool bClipOnly;
			bool noAmmoPickup;
			bool adsFireOnly;
			bool cancelAutoHolsterWhenEmpty;
			bool disableSwitchToWhenEmpty;
			bool suppressAmmoReserveDisplay;
			bool laserSightDuringNightvision;
			bool markableViewmodel;
			bool noDualWield;
			bool flipKillIcon;
			bool bNoPartialReload;
			bool bSegmentedReload;
			bool blocksProne;
			bool silenced;
			bool isRollingGrenade;
			bool projExplosionEffectForceNormalUp;
			bool bProjImpactExplode;
			bool stickToPlayers;
			bool stickToVehicles;
			bool stickToTurrets;
			bool hasDetonator;
			bool disableFiring;
			bool timedDetonation;
			bool rotate;
			bool holdButtonToThrow;
			bool freezeMovementWhenFiring;
			bool thermalScope;
			bool altModeSameWeapon;
			bool turretBarrelSpinEnabled;
			bool missileConeSoundEnabled;
			bool missileConeSoundPitchshiftEnabled;
			bool missileConeSoundCrossfadeEnabled;
			bool offhandHoldIsCancelable;
			bool doNotAllowAttachmentsToOverrideSpread;
			unsigned __int16 stowTag;
			XModel* stowOffsetModel;
		};
#pragma pack(pop)

		struct AnimOverrideEntry
		{
			unsigned __int16 attachment1;
			unsigned __int16 attachment2;
			const char* overrideAnim;
			const char* altmodeAnim;
			unsigned int animTreeType;
			int animTime;
			int altTime;
		};

		struct SoundOverrideEntry
		{
			unsigned __int16 attachment1;
			unsigned __int16 attachment2;
			snd_alias_list_t* overrideSound;
			snd_alias_list_t* altmodeSound;
			unsigned int soundType;
		};

		struct FXOverrideEntry
		{
			unsigned __int16 attachment1;
			unsigned __int16 attachment2;
			FxEffectDef* overrideFX;
			FxEffectDef* altmodeFX;
			unsigned int fxType;
		};

		struct ReloadStateTimerEntry
		{
			int attachment;
			int reloadAddTime;
			int reloadEmptyAddTime;
			// int reloadStartAddTime;
			// one of those is wrong, idk which one
		};

		struct NoteTrackToSoundEntry
		{
			int attachment;
			short* notetrackSoundMapKeys;
			short* notetrackSoundMapValues;
		};

		struct AttachmentDefUnk
		{
			char _pad1[16];
			TracerDef* tracer;
			char _pad2[4];
		};

		struct AttAmmoGeneral
		{
			std::int32_t penetrateType;
			float penetrateMultiplier;
			std::int32_t impactType;
			std::int32_t fireType;
			TracerDef* tracerType;
			bool rifleBullet;
			bool armorPiercing;
			char pad[2];
		};

		struct AttSight
		{
			bool aimDownSight;
			bool adsFire;
			bool rechamberWhileAds;
			bool noAdsWhenMagEmpty;
			bool canHoldBreath;
			bool canVariableZoom;
			bool hideRailWithThisScope;
			// bool useScopeDrift;
			// bool useDualFOV;
		};

		struct AttReload
		{
			bool noPartialReload;
			bool segmentedReload;
		};

		struct AttAddOns
		{
			bool motionTracker;
			bool silenced;
		};

		struct AttGeneral
		{
			bool boltAction;
			bool inheritsPerks;
			bool reticleSpin45;
			char pad[1];
			float enemyCrosshairRange;
			Material* reticleCenter;
			Material* reticleSide;
			int reticleCenterSize;
			int reticleSideSize;
			float moveSpeedScale;
			float adsMoveSpeedScale;
		};

		struct AttAmmunition
		{
			int maxAmmo;
			int startAmmo;
			int clipSize;
		};

		struct AttDamage
		{
			int damage;
			int minDamage;
			int meleeDamage;
			float maxDamageRange;
			float minDamageRange;
			int playerDamage;
			int minPlayerDamage;
		};

		struct AttLocationDamage
		{
			float locNone;
			float locHelmet;
			float locHead;
			float locNeck;
			float locTorsoUpper;
			float locTorsoLower;
			float locRightArmUpper;
			float locRightArmLower;
			float locRightHand;
			float locLeftArmUpper;
			float locLeftArmLower;
			float locLeftHand;
			float locRightLegUpper;
			float locRightLegLower;
			float locRightFoot;
			float locLeftLegUpper;
			float locLeftLegLower;
			float locLeftFoot;
			float locGun;
		};

		struct AttIdleSettings
		{
			float hipIdleAmount;
			float hipIdleSpeed;
			float idleCrouchFactor;
			float idleProneFactor;
			float adsIdleLerpStartTime;
			float adsIdleLerpTime;
		};

		struct AttADSSettings
		{
			float adsSpread;
			float adsAimPitch;
			float adsTransInTime;
			float adsTransOutTime;
			int adsReloadTransTime;
			float adsCrosshairInFrac;
			float adsCrosshairOutFrac;
			float adsZoomFov;
			float adsZoomInFrac;
			float adsZoomOutFrac;
			float adsFovLerpTime;
			// float adsBobFactor;
			// float adsViewBobMult;
			float adsFireRateScale;
			float adsDamageRangeScale;
			float adsFireAnimFrac;
		};

		struct AttScopeDriftSettings
		{
			float fScopeDriftDelay;
			float fScopeDriftLerpInTime;
			float fScopeDriftSteadyTime;
			float fScopeDriftLerpOutTime;
			float fScopeDriftSteadyFactor;
			float fScopeDriftUnsteadyFactor;
		};

		struct AttHipSpread
		{
			float hipSpreadStandMin;
			float hipSpreadDuckedMin;
			float hipSpreadProneMin;
			float hipSpreadMax;
			float hipSpreadDuckedMax;
			float hipSpreadProneMax;
			float hipSpreadFireAdd;
			float hipSpreadTurnAdd;
			float hipSpreadMoveAdd;
			float hipSpreadDecayRate;
			float hipSpreadDuckedDecay;
			float hipSpreadProneDecay;
		};

		struct AttGunKick
		{
			int hipGunKickReducedKickBullets;
			float hipGunKickReducedKickPercent;
			float hipGunKickPitchMin;
			float hipGunKickPitchMax;
			float hipGunKickYawMin;
			float hipGunKickYawMax;
			// float hipGunKickMagMin;
			float hipGunKickAccel;
			float hipGunKickSpeedMax;
			float hipGunKickSpeedDecay;
			float hipGunKickStaticDecay;
			int adsGunKickReducedKickBullets;
			float adsGunKickReducedKickPercent;
			float adsGunKickPitchMin;
			float adsGunKickPitchMax;
			float adsGunKickYawMin;
			float adsGunKickYawMax;
			// float adsGunKickMagMin;
			float adsGunKickAccel;
			float adsGunKickSpeedMax;
			float adsGunKickSpeedDecay;
			float adsGunKickStaticDecay;
		};

		struct AttViewKick
		{
			float hipViewKickPitchMin;
			float hipViewKickPitchMax;
			float hipViewKickYawMin;
			float hipViewKickYawMax;
			// float hipViewKickMagMin;
			float hipViewKickCenterSpeed;
			float adsViewKickPitchMin;
			float adsViewKickPitchMax;
			float adsViewKickYawMin;
			float adsViewKickYawMax;
			// float adsViewKickMagMin;
			float adsViewKickCenterSpeed;
		};

		struct AttADSOverlay
		{
			ADSOverlay overlay;
			bool hybridToggle;
			bool thermalScope;
			bool thermalToggle;
			bool outlineEnemies;
		};

		struct AttUI
		{
			Material* dpadIcon;
			Material* ammoCounterIcon;
			weaponIconRatioType_t dpadIconRatio;
			weaponIconRatioType_t ammoCounterIconRatio;
			ammoCounterClipType_t ammoCounterClip;
		};

		struct AttRumbles
		{
			const char* fireRumble;
			const char* meleeImpactRumble;
		};

		struct AttProjectile
		{
			int explosionRadius;
			int explosionInnerDamage;
			int explosionOuterDamage;
			float damageConeAngle;
			int projectileSpeed;
			int projectileSpeedUp;
			int projectileActivateDist;
			float projectileLifetime;
			XModel* projectileModel;
			int projExplosionType;
			FxEffectDef* projExplosionEffect;
			bool projExplosionEffectForceNormalUp;
			char pad1[3];
			snd_alias_list_t* projExplosionSound;
			FxEffectDef* projDudEffect;
			snd_alias_list_t* projDudSound;
			bool projImpactExplode;
			char pad2[3];
			float destabilizationRateTime;
			float destabilizationCurvatureMax;
			int destabilizeDistance;
			FxEffectDef* projTrailEffect;
			int projIgnitionDelay;
			FxEffectDef* projIgnitionEffect;
			snd_alias_list_t* projIgnitionSound;
		};

		struct AttachmentDef
		{
			const char* szInternalName;
			const char* szDisplayName;
			std::int32_t type;
			std::int32_t weaponType;
			std::int32_t weapClass;
			XModel** worldModels;
			XModel** viewModels;
			XModel** reticleViewModels;
			AttAmmoGeneral* ammogeneral;
			AttSight* sight;
			AttReload* reload;
			AttAddOns* addOns;
			AttGeneral* general;
			AttAmmunition* ammunition;
			AttIdleSettings* idleSettings;
			AttDamage* damage;
			AttLocationDamage* locationDamage;
			AttScopeDriftSettings* scopeDriftSettings;
			AttADSSettings* adsSettings;
			AttADSSettings* adsSettingsMain;
			AttHipSpread* hipSpread;
			AttGunKick* gunKick;
			AttViewKick* viewKick;
			AttADSOverlay* adsOverlay;
			AttUI* ui;
			AttRumbles* rumbles;
			AttProjectile* projectile;
			float ammunitionScale;
			float damageScale;
			float damageScaleMin;
			float stateTimersScale;
			float fireTimersScale;
			float idleSettingsScale;
			float adsSettingsScale;
			float adsSettingsScaleMain;
			float hipSpreadScale;
			float gunKickScale;
			float viewKickScale;
			float viewCenterScale;
			int loadIndex;
			bool hideIronSightsWithThisAttachment;
			bool shareAmmoWithAlt;
			char pad[2];
		};

#pragma pack(push, 4)
		struct WeaponCompleteDef
		{
			const char* szInternalName;
			WeaponDef* weapDef;
			const char* szDisplayName;
			unsigned __int16* hideTags;
			AttachmentDef** scopes;
			AttachmentDef** underBarrels;
			AttachmentDef** others;
			const char** szXAnims;
			unsigned int numAnimOverrides;
			AnimOverrideEntry* animOverrides;
			unsigned int numSoundOverrides;
			SoundOverrideEntry* soundOverrides;
			unsigned int numFXOverrides;
			FXOverrideEntry* fxOverrides;
			unsigned int numReloadStateTimerOverrides;
			ReloadStateTimerEntry* reloadOverrides;
			unsigned int numNotetrackOverrides;
			NoteTrackToSoundEntry* notetrackOverrides;
			float fAdsZoomFov;
			int iAdsTransInTime;
			int iAdsTransOutTime;
			int iClipSize;
			int impactType;
			int iFireTime;
			int iFireTimeAkimbo;
			weaponIconRatioType_t dpadIconRatio;
			float penetrateMultiplier;
			float fAdsViewKickCenterSpeed;
			float fHipViewKickCenterSpeed;
			const char* szAltWeaponName;
			int altWeapon;
			int iAltRaiseTime;
			int iAltRaiseTimeAkimbo;
			Material* killIcon;
			Material* dpadIcon;
			int fireAnimLength;
			int fireAnimLengthAkimbo;
			int iFirstRaiseTime;
			int iFirstRaiseTimeAkimbo;
			int ammoDropStockMax;
			float adsDofStart;
			float adsDofEnd;
			unsigned __int16 accuracyGraphKnotCount[2];
			float(*accuracyGraphKnots[2])[2];
			bool motionTracker;
			bool enhanced;
			bool dpadIconShowsAmmo;
		};
#pragma pack(pop)

		// FxWorld
#pragma pack(push, 4)

		struct FxGlassDef
		{
			float halfThickness;
			float texVecs[2][2];
			GfxColor color;
			Material* material;
			Material* materialShattered;
			PhysPreset* physPreset;
		};

		struct FxSpatialFrame
		{
			float quat[4];
			float origin[3];
		};

		union FxGlassPiecePlace
		{
			struct
			{
				FxSpatialFrame frame;
				float radius;
			};

			unsigned int nextFree;
		};

		struct FxGlassPieceState
		{
			float texCoordOrigin[2];
			unsigned int supportMask;
			unsigned __int16 initIndex;
			unsigned __int16 geoDataStart;
			char defIndex;
			char pad[5];
			char vertCount;
			char holeDataCount;
			char crackDataCount;
			char fanDataCount;
			unsigned __int16 flags;
			float areaX2;
		};

		struct FxGlassPieceDynamics
		{
			int fallTime;
			__int32 physObjId;
			__int32 physJointId;
			float vel[3];
			float avel[3];
		};

		struct FxGlassVertex
		{
			__int16 x;
			__int16 y;
		};

		struct FxGlassHoleHeader
		{
			unsigned __int16 uniqueVertCount;
			char touchVert;
			char pad[1];
		};

		struct FxGlassCrackHeader
		{
			unsigned __int16 uniqueVertCount;
			char beginVertIndex;
			char endVertIndex;
		};

		union FxGlassGeometryData
		{
			FxGlassVertex vert;
			FxGlassHoleHeader hole;
			FxGlassCrackHeader crack;
			char asBytes[4];
			__int16 anonymous[2];
		};

		struct FxGlassInitPieceState
		{
			FxSpatialFrame frame;
			float radius;
			float texCoordOrigin[2];
			unsigned int supportMask;
			float areaX2;
			char defIndex;
			char vertCount;
			char fanDataCount;
			char pad[1];
		};

		struct FxGlassSystem
		{
			int time; // 4
			int prevTime; // 4
			unsigned int defCount; // 4
			unsigned int pieceLimit; // 4
			unsigned int pieceWordCount; // 4
			unsigned int initPieceCount; // 4
			unsigned int cellCount; // 4
			unsigned int activePieceCount; // 4
			unsigned int firstFreePiece; // 4
			unsigned int geoDataLimit; // 4
			unsigned int geoDataCount; // 4
			unsigned int initGeoDataCount; // 4
			FxGlassDef* defs; // 4
			FxGlassPiecePlace* piecePlaces; // 4
			FxGlassPieceState* pieceStates; // 4
			FxGlassPieceDynamics* pieceDynamics; // 4
			FxGlassGeometryData* geoData; // 4
			unsigned int* isInUse; // 4
			unsigned int* cellBits; // 4
			char* visData; // 4
			VecInternal<3>* linkOrg;
			float* halfThickness; // 4
			unsigned __int16* lightingHandles; // 4
			FxGlassInitPieceState* initPieceStates; // 4
			FxGlassGeometryData* initGeoData; // 4
			bool needToCompactData; // 1
			char initCount;
			short pad;
			float effectChanceAccum; // 4
			int lastPieceDeletionTime; // 4
		};

		struct FxWorld
		{
			char* name;
			FxGlassSystem glassSys;
		};

		struct FxImpactEntry
		{
			FxEffectDef* nonflesh[31];
			FxEffectDef* flesh[4];
		};

		struct FxImpactTable
		{
			const char* name;
			FxImpactEntry* table;
		};

		enum LbColType
		{
			LBCOL_TYPE_NUMBER = 0x0,
			LBCOL_TYPE_TIME = 0x1,
			LBCOL_TYPE_LEVELXP = 0x2,
			LBCOL_TYPE_PRESTIGE = 0x3,
			LBCOL_TYPE_BIGNUMBER = 0x4,
			LBCOL_TYPE_PERCENT = 0x5,
			LBCOL_TYPE_TIME_FULL = 0x6,
			LBCOL_TYPE_COUNT = 0x7
		};

		enum LbAggType
		{
			LBAGG_TYPE_MIN = 0x0,
			LBAGG_TYPE_MAX = 0x1,
			LBAGG_TYPE_ADD = 0x2,
			LBAGG_TYPE_REPLACE = 0x3,
			LBAGG_TYPE_COUNT = 0x4
		};

		enum LbUpdateType
		{
			LBUPDATE_TYPE_NORMAL = 0x0,
			LBUPDATE_TYPE_RANK = 0x1,
			LBUPDATE_TYPE_COMBINE = 0x2,
			LBUPDATE_TYPE_COUNT = 0x3,
		};

		struct LbColumnDef
		{
			const char* title; // 0
			int colId; // 4
			int dwColIndex; // 8
			bool hidden; // 12
			const char* statName; // 16
			LbColType type;
			int precision;
			LbAggType agg;
			int uiCalColX;
			int uiCalColY;
		};

		struct LeaderBoardDef
		{
			const char* name;
			unsigned int id;
			int columnCount;
			int dwColumnCount;
			int xpColId;
			LbColumnDef* columns;
			LbUpdateType updateType;
			int trackTypes;
		};
#pragma pack(pop)

		struct GfxImageFileHeader
		{
			char tag[3];
			char version;
			unsigned int flags;
			char format;
			char unused;
			__int16 dimensions[3];
			int fileSizeForPicmip[4];
		};

		union XAssetHeader
		{
			RawFile* rawfile;
			VertexDecl* vertexdecl;
			PixelShader* pixelshader;
			VertexShader* vertexshader;
			MaterialTechniqueSet* techset;
			GfxImage* gfximage;
			Material* material;
			PhysPreset* physpreset;
			PhysCollmap* physcollmap;
			XAnimParts* xanimparts;
			ModelSurface* xsurface;
			clipMap_t* clipmap;
			GfxWorld* gfxworld;
			MapEnts* mapents;
			GlassWorld* glassworld;
			FxWorld* fxworld;
			XModel* xmodel;
			StringTable* stringtable;
			ScriptFile* scriptfile;
			ComWorld* comworld;
			LocalizeEntry* localize;
			SndCurve* soundcurve;
			TracerDef* tracer;
			LeaderBoardDef* leaderboard;
			Font_s* font;
			AttachmentDef* attachment;
			WeaponCompleteDef* weapon;
			FxEffectDef* fx;
			snd_alias_list_t* sound;
			LoadedSound* loadedsound;
			StructuredDataDefSet* structureddatadef;
			menuDef_t* menu;
			MenuList* menuList;
			GfxLightDef* lightdef;
			void* data;
		};

		enum $8C6ECA10AE5C9C0C746CF1CD5CE4FD8D
		{
			CONST_SRC_CODE_LIGHT_POSITION,
			CONST_SRC_CODE_LIGHT_DIFFUSE,
			CONST_SRC_CODE_LIGHT_SPECULAR,
			CONST_SRC_CODE_LIGHT_SPOTDIR,
			CONST_SRC_CODE_LIGHT_SPOTFACTORS,
			CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT,
			CONST_SRC_CODE_PARTICLE_CLOUD_COLOR,
			CONST_SRC_CODE_GAMETIME,
			CONST_SRC_CODE_EYEOFFSET,
			CONST_SRC_CODE_COLOR_SATURATION_R,
			CONST_SRC_CODE_COLOR_SATURATION_G,
			CONST_SRC_CODE_COLOR_SATURATION_B,
			CONST_SRC_CODE_PIXEL_COST_FRACS,
			CONST_SRC_CODE_PIXEL_COST_DECODE,
			CONST_SRC_CODE_FILTER_TAP_0,
			CONST_SRC_CODE_FILTER_TAP_1,
			CONST_SRC_CODE_FILTER_TAP_2,
			CONST_SRC_CODE_FILTER_TAP_3,
			CONST_SRC_CODE_FILTER_TAP_4,
			CONST_SRC_CODE_FILTER_TAP_5,
			CONST_SRC_CODE_FILTER_TAP_6,
			CONST_SRC_CODE_FILTER_TAP_7,
			CONST_SRC_CODE_COLOR_MATRIX_R,
			CONST_SRC_CODE_COLOR_MATRIX_G,
			CONST_SRC_CODE_COLOR_MATRIX_B,
			CONST_SRC_CODE_UNK1,
			CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET,
			CONST_SRC_CODE_RENDER_TARGET_SIZE,
			CONST_SRC_CODE_RENDER_SOURCE_SIZE,
			CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR,
			CONST_SRC_CODE_DOF_EQUATION_SCENE,
			CONST_SRC_CODE_DOF_LERP_SCALE,
			CONST_SRC_CODE_DOF_LERP_BIAS,
			CONST_SRC_CODE_DOF_ROW_DELTA,
			CONST_SRC_CODE_MOTION_MATRIX_X,
			CONST_SRC_CODE_MOTION_MATRIX_Y,
			CONST_SRC_CODE_MOTION_MATRIX_W,
			CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION,
			CONST_SRC_CODE_SHADOWMAP_SCALE,
			CONST_SRC_CODE_ZNEAR,
			CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE,
			CONST_SRC_CODE_DEBUG_BUMPMAP,
			CONST_SRC_CODE_MATERIAL_COLOR,
			CONST_SRC_CODE_FOG,
			CONST_SRC_CODE_FOG_COLOR_LINEAR,
			CONST_SRC_CODE_FOG_COLOR_GAMMA,
			CONST_SRC_CODE_FOG_SUN_CONSTS,
			CONST_SRC_CODE_FOG_SUN_COLOR_LINEAR,
			CONST_SRC_CODE_FOG_SUN_COLOR_GAMMA,
			CONST_SRC_CODE_FOG_SUN_DIR,
			CONST_SRC_CODE_GLOW_SETUP,
			CONST_SRC_CODE_GLOW_APPLY,
			CONST_SRC_CODE_COLOR_BIAS,
			CONST_SRC_CODE_COLOR_TINT_BASE,
			CONST_SRC_CODE_COLOR_TINT_DELTA,
			CONST_SRC_CODE_COLOR_TINT_QUADRATIC_DELTA,
			CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS,
			CONST_SRC_CODE_ENVMAP_PARMS,
			CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST,
			CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST,
			CONST_SRC_CODE_COMPOSITE_FX_DISTORTION,
			CONST_SRC_CODE_POSTFX_FADE_EFFECT,
			CONST_SRC_CODE_VIEWPORT_DIMENSIONS,
			CONST_SRC_CODE_FRAMEBUFFER_READ,
			CONST_SRC_CODE_THERMAL_COLOR_OFFSET,
			CONST_SRC_CODE_PLAYLIST_POPULATION_PARAMS,
			CONST_SRC_CODE_BASE_LIGHTING_COORDS,
			CONST_SRC_CODE_LIGHT_PROBE_AMBIENT,
			CONST_SRC_CODE_NEARPLANE_ORG,
			CONST_SRC_CODE_NEARPLANE_DX,
			CONST_SRC_CODE_NEARPLANE_DY,
			CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE,
			CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET,
			CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0,
			CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX1,
			CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX2,
			CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR0,
			CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR1,
			CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2,
			CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM0,
			CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM1,
			CONST_SRC_CODE_DEPTH_FROM_CLIP,
			CONST_SRC_CODE_CODE_MESH_ARG_0,
			CONST_SRC_CODE_CODE_MESH_ARG_1,
			CONST_SRC_CODE_VIEW_MATRIX,
			CONST_SRC_CODE_INVERSE_VIEW_MATRIX,
			CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX,
			CONST_SRC_CODE_PROJECTION_MATRIX,
			CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX,
			CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX,
			CONST_SRC_CODE_VIEW_PROJECTION_MATRIX,
			CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX,
			CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX,
			CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX,
			CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX,
			CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX,
			CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX,
			CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX,
			CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX,
			CONST_SRC_CODE_WORLD_MATRIX0,
			CONST_SRC_CODE_INVERSE_WORLD_MATRIX0,
			CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0,
			CONST_SRC_CODE_WORLD_VIEW_MATRIX0,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0,
			CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0,
			CONST_SRC_CODE_WORLD_MATRIX1,
			CONST_SRC_CODE_INVERSE_WORLD_MATRIX1,
			CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX1,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX1,
			CONST_SRC_CODE_WORLD_VIEW_MATRIX1,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX1,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX1,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1,
			CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX1,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX1,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1,
			CONST_SRC_CODE_WORLD_MATRIX2,
			CONST_SRC_CODE_INVERSE_WORLD_MATRIX2,
			CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX2,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX2,
			CONST_SRC_CODE_WORLD_VIEW_MATRIX2,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX2,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX2,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2,
			CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX2,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX2,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2,
			CONST_SRC_TOTAL_COUNT,
			CONST_SRC_NONE,
		};
	}
}
