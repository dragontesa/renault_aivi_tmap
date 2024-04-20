﻿// ndds_hancode_table.cpp

#include "ndds_hancode_table.h"

// KSC5601 완성 -> KSSM 조합 

unsigned short NDDS_HANCODE_SINGLE_TABLE[NDDS_HANCODE_SINGLE_TABLE_SIZE] = {

	/*
	0xA4A1 - 0xA4D3
	   ㄱ ㄲ ㄳ ㄴ ㄵ ㄶ ㄷ ㄸ ㄹ ㄺ ㄻ ㄼ ㄽ ㄾ ㄿ 
	ㅀ ㅁ ㅂ ㅃ ㅄ ㅅ ㅆ ㅇ ㅈ ㅉ ㅊ ㅋ ㅌ ㅍ ㅎ ㅏ 
	ㅐ ㅑ ㅒ ㅓ ㅔ ㅕ ㅖ ㅗ ㅘ ㅙ ㅚ ㅛ ㅜ ㅝ ㅞ ㅟ 
	ㅠ ㅡ ㅢ ㅣ
	*/
	       0x8841,0x8c41,0x8444,0x9041,0x8446,0x8447,0x9441,
	0x9841,0x9c41,0x844a,0x844b,0x844c,0x844d,0x844e,0x844f,
	0x8450,0xa041,0xa441,0xa841,0x8454,0xac41,0xb041,0xb441,
	0xb841,0xbc41,0xc041,0xc441,0xc841,0xcc41,0xd041,0x8461,
	0x8481,0x84a1,0x84c1,0x84e1,0x8541,0x8561,0x8581,0x85a1,
	0x85c1,0x85e1,0x8641,0x8661,0x8681,0x86a1,0x86c1,0x86e1,
	0x8741,0x8761,0x8781,0x87a1
};

unsigned short NDDS_HANCODE_TABLE[NDDS_HANCODE_TABLE_SIZE] =
{           
	/*
	0xB0A1 - 0xB0FE
	   가 각 간 갇 갈 갉 갊 감 갑 값 갓 갔 강 갖 갗 
	같 갚 갛 개 객 갠 갤 갬 갭 갯 갰 갱 갸 갹 갼 걀 
	걋 걍 걔 걘 걜 거 걱 건 걷 걸 걺 검 겁 것 겄 겅 
	겆 겉 겊 겋 게 겐 겔 겜 겝 겟 겠 겡 겨 격 겪 견 
	겯 결 겸 겹 겻 겼 경 곁 계 곈 곌 곕 곗 고 곡 곤 
	곧 골 곪 곬 곯 곰 곱 곳 공 곶 과 곽 관 괄 괆 
	*/
		
   0x8861,0x8862,0x8865,0x8868,0x8869,0x886a,0x886b,0x8871,
   0x8873,0x8874,0x8875,0x8876,0x8877,0x8878,0x8879,0x887b,
   0x887c,0x887d,0x8881,0x8882,0x8885,0x8889,0x8891,0x8893,
   0x8895,0x8896,0x8897,0x88a1,0x88a2,0x88a5,0x88a9,0x88b5,
   0x88b7,0x88c1,0x88c5,0x88c9,0x88e1,0x88e2,0x88e5,0x88e8,
   0x88e9,0x88eb,0x88f1,0x88f3,0x88f5,0x88f6,0x88f7,0x88f8,
   0x88fb,0x88fc,0x88fd,0x8941,0x8945,0x8949,0x8951,0x8953,
   0x8955,0x8956,0x8957,0x8961,0x8962,0x8963,0x8965,0x8968,
   0x8969,0x8971,0x8973,0x8975,0x8976,0x8977,0x897b,0x8981,
   0x8985,0x8989,0x8993,0x8995,0x89a1,0x89a2,0x89a5,0x89a8,
   0x89a9,0x89ab,0x89ad,0x89b0,0x89b1,0x89b3,0x89b5,0x89b7,
   0x89b8,0x89c1,0x89c2,0x89c5,0x89c9,0x89cb,

	/*
	0xB1A1 - 0xB1FE
	   괌 괍 괏 광 괘 괜 괠 괩 괬 괭 괴 괵 괸 괼 굄 
	굅 굇 굉 교 굔 굘 굡 굣 구 국 군 굳 굴 굵 굶 굻
	굼 굽 굿 궁 궂 궈 궉 권 궐 궜 궝 궤 궷 귀 귁 귄 
	귈 귐 귑 귓 규 균 귤 그 극 근 귿 글 긁 금 급 긋 
	긍 긔 기 긱 긴 긷 길 긺 김 깁 깃 깅 깆 깊 까 깍 
	깎 깐 깔 깖 깜 깝 깟 깠 깡 깥 깨 깩 깬 깰 깸
	*/
   
   0x89d1,0x89d3,0x89d5,0x89d7,0x89e1,0x89e5,0x89e9,0x89f1,
   0x89f6,0x89f7,0x8a41,0x8a42,0x8a45,0x8a49,0x8a51,0x8a53,
   0x8a55,0x8a57,0x8a61,0x8a65,0x8a69,0x8a73,0x8a75,0x8a81,
   0x8a82,0x8a85,0x8a88,0x8a89,0x8a8a,0x8a8b,0x8a90,0x8a91,
   0x8a93,0x8a95,0x8a97,0x8a98,0x8aa1,0x8aa2,0x8aa5,0x8aa9,
   0x8ab6,0x8ab7,0x8ac1,0x8ad5,0x8ae1,0x8ae2,0x8ae5,0x8ae9,
   0x8af1,0x8af3,0x8af5,0x8b41,0x8b45,0x8b49,0x8b61,0x8b62,
   0x8b65,0x8b68,0x8b69,0x8b6a,0x8b71,0x8b73,0x8b75,0x8b77,
   0x8b81,0x8ba1,0x8ba2,0x8ba5,0x8ba8,0x8ba9,0x8bab,0x8bb1,
   0x8bb3,0x8bb5,0x8bb7,0x8bb8,0x8bbc,0x8c61,0x8c62,0x8c63,
   0x8c65,0x8c69,0x8c6b,0x8c71,0x8c73,0x8c75,0x8c76,0x8c77,
   0x8c7b,0x8c81,0x8c82,0x8c85,0x8c89,0x8c91,

	/*
	0xB2A1 - 0xB2FE
	   깹 깻 깼 깽 꺄 꺅 꺌 꺼 꺽 꺾 껀 껄 껌 껍 껏 
	껐 껑 께 껙 껜 껨 껫 껭 껴 껸 껼 꼇 꼈 꼍 꼐 꼬 
	꼭 꼰 꼲 꼴 꼼 꼽 꼿 꽁 꽂 꽃 꽈 꽉 꽐 꽜 꽝 꽤 
	꽥 꽹 꾀 꾄 꾈 꾐 꾑 꾕 꾜 꾸 꾹 꾼 꿀 꿇 꿈 꿉 
	꿋 꿍 꿎 꿔 꿜 꿨 꿩 꿰 꿱 꿴 꿸 뀀 뀁 뀄 뀌 뀐 
	뀔 뀜 뀝 뀨 끄 끅 끈 끊 끌 끎 끓 끔 끕 끗 끙
	*/
   
   0x8c93,0x8c95,0x8c96,0x8c97,0x8ca1,0x8ca2,0x8ca9,0x8ce1,
   0x8ce2,0x8ce3,0x8ce5,0x8ce9,0x8cf1,0x8cf3,0x8cf5,0x8cf6,
   0x8cf7,0x8d41,0x8d42,0x8d45,0x8d51,0x8d55,0x8d57,0x8d61,
   0x8d65,0x8d69,0x8d75,0x8d76,0x8d7b,0x8d81,0x8da1,0x8da2,
   0x8da5,0x8da7,0x8da9,0x8db1,0x8db3,0x8db5,0x8db7,0x8db8,
   0x8db9,0x8dc1,0x8dc2,0x8dc9,0x8dd6,0x8dd7,0x8de1,0x8de2,
   0x8df7,0x8e41,0x8e45,0x8e49,0x8e51,0x8e53,0x8e57,0x8e61,
   0x8e81,0x8e82,0x8e85,0x8e89,0x8e90,0x8e91,0x8e93,0x8e95,
   0x8e97,0x8e98,0x8ea1,0x8ea9,0x8eb6,0x8eb7,0x8ec1,0x8ec2,
   0x8ec5,0x8ec9,0x8ed1,0x8ed3,0x8ed6,0x8ee1,0x8ee5,0x8ee9,
   0x8ef1,0x8ef3,0x8f41,0x8f61,0x8f62,0x8f65,0x8f67,0x8f69,
   0x8f6b,0x8f70,0x8f71,0x8f73,0x8f75,0x8f77,

	/*
	0xB3A1 - 0xB3FE
	   끝 끼 끽 낀 낄 낌 낍 낏 낑 나 낙 낚 난 낟 날 
	낡 낢 남 납 낫 났 낭 낮 낯 낱 낳 내 낵 낸 낼 냄 
	냅 냇 냈 냉 냐 냑 냔 냘 냠 냥 너 넉 넋 넌 널 넒 
	넓 넘 넙 넛 넜 넝 넣 네 넥 넨 넬 넴 넵 넷 넸 넹 
	녀 녁 년 녈 념 녑 녔 녕 녘 녜 녠 노 녹 논 놀 놂 
	놈 놉 놋 농 높 놓 놔 놘 놜 놨 뇌 뇐 뇔 뇜 뇝 
	*/
   
   0x8f7b,0x8fa1,0x8fa2,0x8fa5,0x8fa9,0x8fb1,0x8fb3,0x8fb5,
   0x8fb7,0x9061,0x9062,0x9063,0x9065,0x9068,0x9069,0x906a,
   0x906b,0x9071,0x9073,0x9075,0x9076,0x9077,0x9078,0x9079,
   0x907b,0x907d,0x9081,0x9082,0x9085,0x9089,0x9091,0x9093,
   0x9095,0x9096,0x9097,0x90a1,0x90a2,0x90a5,0x90a9,0x90b1,
   0x90b7,0x90e1,0x90e2,0x90e4,0x90e5,0x90e9,0x90eb,0x90ec,
   0x90f1,0x90f3,0x90f5,0x90f6,0x90f7,0x90fd,0x9141,0x9142,
   0x9145,0x9149,0x9151,0x9153,0x9155,0x9156,0x9157,0x9161,
   0x9162,0x9165,0x9169,0x9171,0x9173,0x9176,0x9177,0x917a,
   0x9181,0x9185,0x91a1,0x91a2,0x91a5,0x91a9,0x91ab,0x91b1,
   0x91b3,0x91b5,0x91b7,0x91bc,0x91bd,0x91c1,0x91c5,0x91c9,
   0x91d6,0x9241,0x9245,0x9249,0x9251,0x9253,

	/*
	0xB4A1 - 0xB4FE
	   뇟 뇨 뇩 뇬 뇰 뇹 뇻 뇽 누 눅 눈 눋 눌 눔 눕 
	눗 눙 눠 눴 눼 뉘 뉜 뉠 뉨 뉩 뉴 뉵 뉼 늄 늅 늉 
	느 늑 는 늘 늙 늚 늠 늡 늣 능 늦 늪 늬 늰 늴 니 
	닉 닌 닐 닒 님 닙 닛 닝 닢 다 닥 닦 단 닫 달 닭 
	닮 닯 닳 담 답 닷 닸 당 닺 닻 닿 대 댁 댄 댈 댐 
	댑 댓 댔 댕 댜 더 덕 덖 던 덛 덜 덞 덟 덤 덥 
	*/
   
   0x9255,0x9261,0x9262,0x9265,0x9269,0x9273,0x9275,0x9277,
   0x9281,0x9282,0x9285,0x9288,0x9289,0x9291,0x9293,0x9295,
   0x9297,0x92a1,0x92b6,0x92c1,0x92e1,0x92e5,0x92e9,0x92f1,
   0x92f3,0x9341,0x9342,0x9349,0x9351,0x9353,0x9357,0x9361,
   0x9362,0x9365,0x9369,0x936a,0x936b,0x9371,0x9373,0x9375,
   0x9377,0x9378,0x937c,0x9381,0x9385,0x9389,0x93a1,0x93a2,
   0x93a5,0x93a9,0x93af,0x93b1,0x93b3,0x93b5,0x93b7,0x93bc,
   0x9461,0x9462,0x9463,0x9465,0x9468,0x9469,0x946a,0x946b,
   0x946c,0x9470,0x9471,0x9473,0x9475,0x9476,0x9477,0x9478,
   0x9479,0x947d,0x9481,0x9482,0x9485,0x9489,0x9491,0x9493,
   0x9495,0x9496,0x9497,0x94a1,0x94e1,0x94e2,0x94e3,0x94e5,
   0x94e8,0x94e9,0x94eb,0x94ec,0x94f1,0x94f3,

	/*
	0xB5A1 - 0xB5FE
	   덧 덩 덫 덮 데 덱 덴 델 뎀 뎁 뎃 뎄 뎅 뎌 뎐 
	뎔 뎠 뎡 뎨 뎬 도 독 돈 돋 돌 돎 돐 돔 돕 돗 동 
	돛 돝 돠 돤 돨 돼 됐 되 된 될 됨 됩 됫 됴 두 둑 
	둔 둘 둠 둡 둣 둥 둬 뒀 뒈 뒝 뒤 뒨 뒬 뒵 뒷 뒹 
	듀 듄 듈 듐 듕 드 득 든 듣 들 듦 듬 듭 듯 등 듸 
	디 딕 딘 딛 딜 딤 딥 딧 딨 딩 딪 따 딱 딴 딸 
	*/
   
   0x94f5,0x94f7,0x94f9,0x94fc,0x9541,0x9542,0x9545,0x9549,
   0x9551,0x9553,0x9555,0x9556,0x9557,0x9561,0x9565,0x9569,
   0x9576,0x9577,0x9581,0x9585,0x95a1,0x95a2,0x95a5,0x95a8,
   0x95a9,0x95ab,0x95ad,0x95b1,0x95b3,0x95b5,0x95b7,0x95b9,
   0x95bb,0x95c1,0x95c5,0x95c9,0x95e1,0x95f6,0x9641,0x9645,
   0x9649,0x9651,0x9653,0x9655,0x9661,0x9681,0x9682,0x9685,
   0x9689,0x9691,0x9693,0x9695,0x9697,0x96a1,0x96b6,0x96c1,
   0x96d7,0x96e1,0x96e5,0x96e9,0x96f3,0x96f5,0x96f7,0x9741,
   0x9745,0x9749,0x9751,0x9757,0x9761,0x9762,0x9765,0x9768,
   0x9769,0x976b,0x9771,0x9773,0x9775,0x9777,0x9781,0x97a1,
   0x97a2,0x97a5,0x97a8,0x97a9,0x97b1,0x97b3,0x97b5,0x97b6,
   0x97b7,0x97b8,0x9861,0x9862,0x9865,0x9869,

	/*
	0xB6A1 - 0xB6FE
	   땀 땁 땃 땄 땅 땋 때 땍 땐 땔 땜 땝 땟 땠 땡 
	떠 떡 떤 떨 떪 떫 떰 떱 떳 떴 떵 떻 떼 떽 뗀 뗄 
	뗌 뗍 뗏 뗐 뗑 뗘 뗬 또 똑 똔 똘 똥 똬 똴 뙈 뙤 
	뙨 뚜 뚝 뚠 뚤 뚫 뚬 뚱 뛔 뛰 뛴 뛸 뜀 뜁 뜅 뜨 
	뜩 뜬 뜯 뜰 뜸 뜹 뜻 띄 띈 띌 띔 띕 띠 띤 띨 띰 
	띱 띳 띵 라 락 란 랄 람 랍 랏 랐 랑 랒 랖 랗 
	*/
   
   0x9871,0x9873,0x9875,0x9876,0x9877,0x987d,0x9881,0x9882,
   0x9885,0x9889,0x9891,0x9893,0x9895,0x9896,0x9897,0x98e1,
   0x98e2,0x98e5,0x98e9,0x98eb,0x98ec,0x98f1,0x98f3,0x98f5,
   0x98f6,0x98f7,0x98fd,0x9941,0x9942,0x9945,0x9949,0x9951,
   0x9953,0x9955,0x9956,0x9957,0x9961,0x9976,0x99a1,0x99a2,
   0x99a5,0x99a9,0x99b7,0x99c1,0x99c9,0x99e1,0x9a41,0x9a45,
   0x9a81,0x9a82,0x9a85,0x9a89,0x9a90,0x9a91,0x9a97,0x9ac1,
   0x9ae1,0x9ae5,0x9ae9,0x9af1,0x9af3,0x9af7,0x9b61,0x9b62,
   0x9b65,0x9b68,0x9b69,0x9b71,0x9b73,0x9b75,0x9b81,0x9b85,
   0x9b89,0x9b91,0x9b93,0x9ba1,0x9ba5,0x9ba9,0x9bb1,0x9bb3,
   0x9bb5,0x9bb7,0x9c61,0x9c62,0x9c65,0x9c69,0x9c71,0x9c73,
   0x9c75,0x9c76,0x9c77,0x9c78,0x9c7c,0x9c7d,

	/*
	0xB7A1 - 0xB7FE
	   래 랙 랜 랠 램 랩 랫 랬 랭 랴 략 랸 럇 량 러 
	럭 런 럴 럼 럽 럿 렀 렁 렇 레 렉 렌 렐 렘 렙 렛 
	렝 려 력 련 렬 렴 렵 렷 렸 령 례 롄 롑 롓 로 록 
	론 롤 롬 롭 롯 롱 롸 롼 뢍 뢨 뢰 뢴 뢸 룀 룁 룃 
	룅 료 룐 룔 룝 룟 룡 루 룩 룬 룰 룸 룹 룻 룽 뤄 
	뤘 뤠 뤼 뤽 륀 륄 륌 륏 륑 류 륙 륜 률 륨 륩 
	*/
   
   0x9c81,0x9c82,0x9c85,0x9c89,0x9c91,0x9c93,0x9c95,0x9c96,
   0x9c97,0x9ca1,0x9ca2,0x9ca5,0x9cb5,0x9cb7,0x9ce1,0x9ce2,
   0x9ce5,0x9ce9,0x9cf1,0x9cf3,0x9cf5,0x9cf6,0x9cf7,0x9cfd,
   0x9d41,0x9d42,0x9d45,0x9d49,0x9d51,0x9d53,0x9d55,0x9d57,
   0x9d61,0x9d62,0x9d65,0x9d69,0x9d71,0x9d73,0x9d75,0x9d76,
   0x9d77,0x9d81,0x9d85,0x9d93,0x9d95,0x9da1,0x9da2,0x9da5,
   0x9da9,0x9db1,0x9db3,0x9db5,0x9db7,0x9dc1,0x9dc5,0x9dd7,
   0x9df6,0x9e41,0x9e45,0x9e49,0x9e51,0x9e53,0x9e55,0x9e57,
   0x9e61,0x9e65,0x9e69,0x9e73,0x9e75,0x9e77,0x9e81,0x9e82,
   0x9e85,0x9e89,0x9e91,0x9e93,0x9e95,0x9e97,0x9ea1,0x9eb6,
   0x9ec1,0x9ee1,0x9ee2,0x9ee5,0x9ee9,0x9ef1,0x9ef5,0x9ef7,
   0x9f41,0x9f42,0x9f45,0x9f49,0x9f51,0x9f53,

	/*
	0xB8A1 - 0xB8FE
	   륫 륭 르 륵 른 를 름 릅 릇 릉 릊 릍 릎 리 릭 
	린 릴 림 립 릿 링 마 막 만 많 맏 말 맑 맒 맘 맙 
	맛 망 맞 맡 맣 매 맥 맨 맬 맴 맵 맷 맸 맹 맺 먀 
	먁 먈 먕 머 먹 먼 멀 멂 멈 멉 멋 멍 멎 멓 메 멕 
	멘 멜 멤 멥 멧 멨 멩 며 멱 면 멸 몃 몄 명 몇 몌 
	모 목 몫 몬 몰 몲 몸 몹 못 몽 뫄 뫈 뫘 뫙 뫼 
	*/
   
   0x9f55,0x9f57,0x9f61,0x9f62,0x9f65,0x9f69,0x9f71,0x9f73,
   0x9f75,0x9f77,0x9f78,0x9f7b,0x9f7c,0x9fa1,0x9fa2,0x9fa5,
   0x9fa9,0x9fb1,0x9fb3,0x9fb5,0x9fb7,0xa061,0xa062,0xa065,
   0xa067,0xa068,0xa069,0xa06a,0xa06b,0xa071,0xa073,0xa075,
   0xa077,0xa078,0xa07b,0xa07d,0xa081,0xa082,0xa085,0xa089,
   0xa091,0xa093,0xa095,0xa096,0xa097,0xa098,0xa0a1,0xa0a2,
   0xa0a9,0xa0b7,0xa0e1,0xa0e2,0xa0e5,0xa0e9,0xa0eb,0xa0f1,
   0xa0f3,0xa0f5,0xa0f7,0xa0f8,0xa0fd,0xa141,0xa142,0xa145,
   0xa149,0xa151,0xa153,0xa155,0xa156,0xa157,0xa161,0xa162,
   0xa165,0xa169,0xa175,0xa176,0xa177,0xa179,0xa181,0xa1a1,
   0xa1a2,0xa1a4,0xa1a5,0xa1a9,0xa1ab,0xa1b1,0xa1b3,0xa1b5,
   0xa1b7,0xa1c1,0xa1c5,0xa1d6,0xa1d7,0xa241,

	/*
	0xB9A1 - 0xB9FE
	   묀 묄 묍 묏 묑 묘 묜 묠 묩 묫 무 묵 묶 문 묻 
	물 묽 묾 뭄 뭅 뭇 뭉 뭍 뭏 뭐 뭔 뭘 뭡 뭣 뭬 뮈 
	뮌 뮐 뮤 뮨 뮬 뮴 뮷 므 믄 믈 믐 믓 미 믹 민 믿 
	밀 밂 밈 밉 밋 밌 밍 및 밑 바 박 밖 밗 반 받 발 
	밝 밞 밟 밤 밥 밧 방 밭 배 백 밴 밸 뱀 뱁 뱃 뱄 
	뱅 뱉 뱌 뱍 뱐 뱝 버 벅 번 벋 벌 벎 범 법 벗
	*/
   
   0xa245,0xa249,0xa253,0xa255,0xa257,0xa261,0xa265,0xa269,
   0xa273,0xa275,0xa281,0xa282,0xa283,0xa285,0xa288,0xa289,
   0xa28a,0xa28b,0xa291,0xa293,0xa295,0xa297,0xa29b,0xa29d,
   0xa2a1,0xa2a5,0xa2a9,0xa2b3,0xa2b5,0xa2c1,0xa2e1,0xa2e5,
   0xa2e9,0xa341,0xa345,0xa349,0xa351,0xa355,0xa361,0xa365,
   0xa369,0xa371,0xa375,0xa3a1,0xa3a2,0xa3a5,0xa3a8,0xa3a9,
   0xa3ab,0xa3b1,0xa3b3,0xa3b5,0xa3b6,0xa3b7,0xa3b9,0xa3bb,
   0xa461,0xa462,0xa463,0xa464,0xa465,0xa468,0xa469,0xa46a,
   0xa46b,0xa46c,0xa471,0xa473,0xa475,0xa477,0xa47b,0xa481,
   0xa482,0xa485,0xa489,0xa491,0xa493,0xa495,0xa496,0xa497,
   0xa49b,0xa4a1,0xa4a2,0xa4a5,0xa4b3,0xa4e1,0xa4e2,0xa4e5,
   0xa4e8,0xa4e9,0xa4eb,0xa4f1,0xa4f3,0xa4f5,

	/*
	0xBAA1 - 0xBAFE
	   벙 벚 베 벡 벤 벧 벨 벰 벱 벳 벴 벵 벼 벽 변 
	별 볍 볏 볐 병 볕 볘 볜 보 복 볶 본 볼 봄 봅 봇 
	봉 봐 봔 봤 봬 뵀 뵈 뵉 뵌 뵐 뵘 뵙 뵤 뵨 부 북 
	분 붇 불 붉 붊 붐 붑 붓 붕 붙 붚 붜 붤 붰 붸 뷔 
	뷕 뷘 뷜 뷩 뷰 뷴 뷸 븀 븃 븅 브 븍 븐 블 븜 븝 
	븟 비 빅 빈 빌 빎 빔 빕 빗 빙 빚 빛 빠 빡 빤 
	*/
   
   0xa4f7,0xa4f8,0xa541,0xa542,0xa545,0xa548,0xa549,0xa551,
   0xa553,0xa555,0xa556,0xa557,0xa561,0xa562,0xa565,0xa569,
   0xa573,0xa575,0xa576,0xa577,0xa57b,0xa581,0xa585,0xa5a1,
   0xa5a2,0xa5a3,0xa5a5,0xa5a9,0xa5b1,0xa5b3,0xa5b5,0xa5b7,
   0xa5c1,0xa5c5,0xa5d6,0xa5e1,0xa5f6,0xa641,0xa642,0xa645,
   0xa649,0xa651,0xa653,0xa661,0xa665,0xa681,0xa682,0xa685,
   0xa688,0xa689,0xa68a,0xa68b,0xa691,0xa693,0xa695,0xa697,
   0xa69b,0xa69c,0xa6a1,0xa6a9,0xa6b6,0xa6c1,0xa6e1,0xa6e2,
   0xa6e5,0xa6e9,0xa6f7,0xa741,0xa745,0xa749,0xa751,0xa755,
   0xa757,0xa761,0xa762,0xa765,0xa769,0xa771,0xa773,0xa775,
   0xa7a1,0xa7a2,0xa7a5,0xa7a9,0xa7ab,0xa7b1,0xa7b3,0xa7b5,
   0xa7b7,0xa7b8,0xa7b9,0xa861,0xa862,0xa865,

	/*
	0xBBA1 - 0xBBFE
	   빨 빪 빰 빱 빳 빴 빵 빻 빼 빽 뺀 뺄 뺌 뺍 뺏 
	뺐 뺑 뺘 뺙 뺨 뻐 뻑 뻔 뻗 뻘 뻠 뻣 뻤 뻥 뻬 뼁 
	뼈 뼉 뼘 뼙 뼛 뼜 뼝 뽀 뽁 뽄 뽈 뽐 뽑 뽕 뾔 뾰 
	뿅 뿌 뿍 뿐 뿔 뿜 뿟 뿡 쀼 쁑 쁘 쁜 쁠 쁨 쁩 삐 
	삑 삔 삘 삠 삡 삣 삥 사 삭 삯 산 삳 살 삵 삶 삼 
	삽 삿 샀 상 샅 새 색 샌 샐 샘 샙 샛 샜 생 샤 
	*/
   
   0xa869,0xa86b,0xa871,0xa873,0xa875,0xa876,0xa877,0xa87d,
   0xa881,0xa882,0xa885,0xa889,0xa891,0xa893,0xa895,0xa896,
   0xa897,0xa8a1,0xa8a2,0xa8b1,0xa8e1,0xa8e2,0xa8e5,0xa8e8,
   0xa8e9,0xa8f1,0xa8f5,0xa8f6,0xa8f7,0xa941,0xa957,0xa961,
   0xa962,0xa971,0xa973,0xa975,0xa976,0xa977,0xa9a1,0xa9a2,
   0xa9a5,0xa9a9,0xa9b1,0xa9b3,0xa9b7,0xaa41,0xaa61,0xaa77,
   0xaa81,0xaa82,0xaa85,0xaa89,0xaa91,0xaa95,0xaa97,0xab41,
   0xab57,0xab61,0xab65,0xab69,0xab71,0xab73,0xaba1,0xaba2,
   0xaba5,0xaba9,0xabb1,0xabb3,0xabb5,0xabb7,0xac61,0xac62,
   0xac64,0xac65,0xac68,0xac69,0xac6a,0xac6b,0xac71,0xac73,
   0xac75,0xac76,0xac77,0xac7b,0xac81,0xac82,0xac85,0xac89,
   0xac91,0xac93,0xac95,0xac96,0xac97,0xaca1,

	/*
	0xBCA1 - 0xBCFE
	   샥 샨 샬 샴 샵 샷 샹 섀 섄 섈 섐 섕 서 석 섞 
	섟 선 섣 설 섦 섧 섬 섭 섯 섰 성 섶 세 섹 센 셀 
	셈 셉 셋 셌 셍 셔 셕 션 셜 셤 셥 셧 셨 셩 셰 셴 
	셸 솅 소 속 솎 손 솔 솖 솜 솝 솟 송 솥 솨 솩 솬 
	솰 솽 쇄 쇈 쇌 쇔 쇗 쇘 쇠 쇤 쇨 쇰 쇱 쇳 쇼 쇽 
	숀 숄 숌 숍 숏 숑 수 숙 순 숟 술 숨 숩 숫 숭 
	*/
   
   0xaca2,0xaca5,0xaca9,0xacb1,0xacb3,0xacb5,0xacb7,0xacc1,
   0xacc5,0xacc9,0xacd1,0xacd7,0xace1,0xace2,0xace3,0xace4,
   0xace5,0xace8,0xace9,0xaceb,0xacec,0xacf1,0xacf3,0xacf5,
   0xacf6,0xacf7,0xacfc,0xad41,0xad42,0xad45,0xad49,0xad51,
   0xad53,0xad55,0xad56,0xad57,0xad61,0xad62,0xad65,0xad69,
   0xad71,0xad73,0xad75,0xad76,0xad77,0xad81,0xad85,0xad89,
   0xad97,0xada1,0xada2,0xada3,0xada5,0xada9,0xadab,0xadb1,
   0xadb3,0xadb5,0xadb7,0xadbb,0xadc1,0xadc2,0xadc5,0xadc9,
   0xadd7,0xade1,0xade5,0xade9,0xadf1,0xadf5,0xadf6,0xae41,
   0xae45,0xae49,0xae51,0xae53,0xae55,0xae61,0xae62,0xae65,
   0xae69,0xae71,0xae73,0xae75,0xae77,0xae81,0xae82,0xae85,
   0xae88,0xae89,0xae91,0xae93,0xae95,0xae97,

	/*
	0xBDA1 - 0xBDFE
	   숯 숱 숲 숴 쉈 쉐 쉑 쉔 쉘 쉠 쉥 쉬 쉭 쉰 쉴 
	쉼 쉽 쉿 슁 슈 슉 슐 슘 슛 슝 스 슥 슨 슬 슭 슴 
	습 슷 승 시 식 신 싣 실 싫 심 십 싯 싱 싶 싸 싹 
	싻 싼 쌀 쌈 쌉 쌌 쌍 쌓 쌔 쌕 쌘 쌜 쌤 쌥 쌨 쌩 
	썅 써 썩 썬 썰 썲 썸 썹 썼 썽 쎄 쎈 쎌 쏀 쏘 쏙 
	쏜 쏟 쏠 쏢 쏨 쏩 쏭 쏴 쏵 쏸 쐈 쐐 쐤 쐬 쐰 
	*/
   
   0xae99,0xae9b,0xae9c,0xaea1,0xaeb6,0xaec1,0xaec2,0xaec5,
   0xaec9,0xaed1,0xaed7,0xaee1,0xaee2,0xaee5,0xaee9,0xaef1,
   0xaef3,0xaef5,0xaef7,0xaf41,0xaf42,0xaf49,0xaf51,0xaf55,
   0xaf57,0xaf61,0xaf62,0xaf65,0xaf69,0xaf6a,0xaf71,0xaf73,
   0xaf75,0xaf77,0xafa1,0xafa2,0xafa5,0xafa8,0xafa9,0xafb0,
   0xafb1,0xafb3,0xafb5,0xafb7,0xafbc,0xb061,0xb062,0xb064,
   0xb065,0xb069,0xb071,0xb073,0xb076,0xb077,0xb07d,0xb081,
   0xb082,0xb085,0xb089,0xb091,0xb093,0xb096,0xb097,0xb0b7,
   0xb0e1,0xb0e2,0xb0e5,0xb0e9,0xb0eb,0xb0f1,0xb0f3,0xb0f6,
   0xb0f7,0xb141,0xb145,0xb149,0xb157,0xb1a1,0xb1a2,0xb1a5,
   0xb1a8,0xb1a9,0xb1ab,0xb1b1,0xb1b3,0xb1b7,0xb1c1,0xb1c2,
   0xb1c5,0xb1d6,0xb1e1,0xb1f6,0xb241,0xb245,

	/*
	0xBEA1 - 0xBEFE
	   쐴 쐼 쐽 쑈 쑤 쑥 쑨 쑬 쑴 쑵 쑹 쒀 쒔 쒜 쒸 
	쒼 쓩 쓰 쓱 쓴 쓸 쓺 쓿 씀 씁 씌 씐 씔 씜 씨 씩 
	씬 씰 씸 씹 씻 씽 아 악 안 앉 않 알 앍 앎 앓 암 
	압 앗 았 앙 앝 앞 애 액 앤 앨 앰 앱 앳 앴 앵 야 
	약 얀 얄 얇 얌 얍 얏 양 얕 얗 얘 얜 얠 얩 어 억 
	언 얹 얻 얼 얽 얾 엄 업 없 엇 었 엉 엊 엌 엎 
	*/
   
   0xb249,0xb251,0xb253,0xb261,0xb281,0xb282,0xb285,0xb289,
   0xb291,0xb293,0xb297,0xb2a1,0xb2b6,0xb2c1,0xb2e1,0xb2e5,
   0xb357,0xb361,0xb362,0xb365,0xb369,0xb36b,0xb370,0xb371,
   0xb373,0xb381,0xb385,0xb389,0xb391,0xb3a1,0xb3a2,0xb3a5,
   0xb3a9,0xb3b1,0xb3b3,0xb3b5,0xb3b7,0xb461,0xb462,0xb465,
   0xb466,0xb467,0xb469,0xb46a,0xb46b,0xb470,0xb471,0xb473,
   0xb475,0xb476,0xb477,0xb47b,0xb47c,0xb481,0xb482,0xb485,
   0xb489,0xb491,0xb493,0xb495,0xb496,0xb497,0xb4a1,0xb4a2,
   0xb4a5,0xb4a9,0xb4ac,0xb4b1,0xb4b3,0xb4b5,0xb4b7,0xb4bb,
   0xb4bd,0xb4c1,0xb4c5,0xb4c9,0xb4d3,0xb4e1,0xb4e2,0xb4e5,
   0xb4e6,0xb4e8,0xb4e9,0xb4ea,0xb4eb,0xb4f1,0xb4f3,0xb4f4,
   0xb4f5,0xb4f6,0xb4f7,0xb4f8,0xb4fa,0xb4fc,

	/*
	0xBFA1 - 0xBFFE
	   에 엑 엔 엘 엠 엡 엣 엥 여 역 엮 연 열 엶 엷 
	염 엽 엾 엿 였 영 옅 옆 옇 예 옌 옐 옘 옙 옛 옜 
	오 옥 온 올 옭 옮 옰 옳 옴 옵 옷 옹 옻 와 왁 완 
	왈 왐 왑 왓 왔 왕 왜 왝 왠 왬 왯 왱 외 왹 왼 욀 
	욈 욉 욋 욍 요 욕 욘 욜 욤 욥 욧 용 우 욱 운 울 
	욹 욺 움 웁 웃 웅 워 웍 원 월 웜 웝 웠 웡 웨 
	*/
   
   0xb541,0xb542,0xb545,0xb549,0xb551,0xb553,0xb555,0xb557,
   0xb561,0xb562,0xb563,0xb565,0xb569,0xb56b,0xb56c,0xb571,
   0xb573,0xb574,0xb575,0xb576,0xb577,0xb57b,0xb57c,0xb57d,
   0xb581,0xb585,0xb589,0xb591,0xb593,0xb595,0xb596,0xb5a1,
   0xb5a2,0xb5a5,0xb5a9,0xb5aa,0xb5ab,0xb5ad,0xb5b0,0xb5b1,
   0xb5b3,0xb5b5,0xb5b7,0xb5b9,0xb5c1,0xb5c2,0xb5c5,0xb5c9,
   0xb5d1,0xb5d3,0xb5d5,0xb5d6,0xb5d7,0xb5e1,0xb5e2,0xb5e5,
   0xb5f1,0xb5f5,0xb5f7,0xb641,0xb642,0xb645,0xb649,0xb651,
   0xb653,0xb655,0xb657,0xb661,0xb662,0xb665,0xb669,0xb671,
   0xb673,0xb675,0xb677,0xb681,0xb682,0xb685,0xb689,0xb68a,
   0xb68b,0xb691,0xb693,0xb695,0xb697,0xb6a1,0xb6a2,0xb6a5,
   0xb6a9,0xb6b1,0xb6b3,0xb6b6,0xb6b7,0xb6c1,

	/*
	0xC0A1 - 0xC0FE
	   웩 웬 웰 웸 웹 웽 위 윅 윈 윌 윔 윕 윗 윙 유 
	육 윤 율 윰 윱 윳 융 윷 으 윽 은 을 읊 음 읍 읏 
	응 읒 읓 읔 읕 읖 읗 의 읜 읠 읨 읫 이 익 인 일 
	읽 읾 잃 임 입 잇 있 잉 잊 잎 자 작 잔 잖 잗 잘 
	잚 잠 잡 잣 잤 장 잦 재 잭 잰 잴 잼 잽 잿 쟀 쟁 
	쟈 쟉 쟌 쟎 쟐 쟘 쟝 쟤 쟨 쟬 저 적 전 절 젊 
	*/
   
   0xb6c2,0xb6c5,0xb6c9,0xb6d1,0xb6d3,0xb6d7,0xb6e1,0xb6e2,
   0xb6e5,0xb6e9,0xb6f1,0xb6f3,0xb6f5,0xb6f7,0xb741,0xb742,
   0xb745,0xb749,0xb751,0xb753,0xb755,0xb757,0xb759,0xb761,
   0xb762,0xb765,0xb769,0xb76f,0xb771,0xb773,0xb775,0xb777,
   0xb778,0xb779,0xb77a,0xb77b,0xb77c,0xb77d,0xb781,0xb785,
   0xb789,0xb791,0xb795,0xb7a1,0xb7a2,0xb7a5,0xb7a9,0xb7aa,
   0xb7ab,0xb7b0,0xb7b1,0xb7b3,0xb7b5,0xb7b6,0xb7b7,0xb7b8,
   0xb7bc,0xb861,0xb862,0xb865,0xb867,0xb868,0xb869,0xb86b,
   0xb871,0xb873,0xb875,0xb876,0xb877,0xb878,0xb881,0xb882,
   0xb885,0xb889,0xb891,0xb893,0xb895,0xb896,0xb897,0xb8a1,
   0xb8a2,0xb8a5,0xb8a7,0xb8a9,0xb8b1,0xb8b7,0xb8c1,0xb8c5,
   0xb8c9,0xb8e1,0xb8e2,0xb8e5,0xb8e9,0xb8eb,

	/*
	0xC1A1 - 0xC1FE
	   점 접 젓 정 젖 제 젝 젠 젤 젬 젭 젯 젱 져 젼 
	졀 졈 졉 졌 졍 졔 조 족 존 졸 졺 좀 좁 좃 종 좆 
	좇 좋 좌 좍 좔 좝 좟 좡 좨 좼 좽 죄 죈 죌 죔 죕 
	죗 죙 죠 죡 죤 죵 주 죽 준 줄 줅 줆 줌 줍 줏 중 
	줘 줬 줴 쥐 쥑 쥔 쥘 쥠 쥡 쥣 쥬 쥰 쥴 쥼 즈 즉 
	즌 즐 즘 즙 즛 증 지 직 진 짇 질 짊 짐 집 짓 
	*/
   
   0xb8f1,0xb8f3,0xb8f5,0xb8f7,0xb8f8,0xb941,0xb942,0xb945,
   0xb949,0xb951,0xb953,0xb955,0xb957,0xb961,0xb965,0xb969,
   0xb971,0xb973,0xb976,0xb977,0xb981,0xb9a1,0xb9a2,0xb9a5,
   0xb9a9,0xb9ab,0xb9b1,0xb9b3,0xb9b5,0xb9b7,0xb9b8,0xb9b9,
   0xb9bd,0xb9c1,0xb9c2,0xb9c9,0xb9d3,0xb9d5,0xb9d7,0xb9e1,
   0xb9f6,0xb9f7,0xba41,0xba45,0xba49,0xba51,0xba53,0xba55,
   0xba57,0xba61,0xba62,0xba65,0xba77,0xba81,0xba82,0xba85,
   0xba89,0xba8a,0xba8b,0xba91,0xba93,0xba95,0xba97,0xbaa1,
   0xbab6,0xbac1,0xbae1,0xbae2,0xbae5,0xbae9,0xbaf1,0xbaf3,
   0xbaf5,0xbb41,0xbb45,0xbb49,0xbb51,0xbb61,0xbb62,0xbb65,
   0xbb69,0xbb71,0xbb73,0xbb75,0xbb77,0xbba1,0xbba2,0xbba5,
   0xbba8,0xbba9,0xbbab,0xbbb1,0xbbb3,0xbbb5,

	/*
	0xC2A1 - 0xC2FE
	징 짖 짙 짚 짜 짝 짠 짢 짤 짧 짬 짭 짯 짰 짱 
	째 짹 짼 쨀 쨈 쨉 쨋 쨌 쨍 쨔 쨘 쨩 쩌 쩍 쩐 쩔 
	쩜 쩝 쩟 쩠 쩡 쩨 쩽 쪄 쪘 쪼 쪽 쫀 쫄 쫌 쫍 쫏 
	쫑 쫓 쫘 쫙 쫠 쫬 쫴 쬈 쬐 쬔 쬘 쬠 쬡 쭁 쭈 쭉 
	쭌 쭐 쭘 쭙 쭝 쭤 쭸 쭹 쮜 쮸 쯔 쯤 쯧 쯩 찌 찍 
	찐 찔 찜 찝 찡 찢 찧 차 착 찬 찮 찰 참 찹 찻 
	*/
   
   0xbbb7,0xbbb8,0xbbbb,0xbbbc,0xbc61,0xbc62,0xbc65,0xbc67,
   0xbc69,0xbc6c,0xbc71,0xbc73,0xbc75,0xbc76,0xbc77,0xbc81,
   0xbc82,0xbc85,0xbc89,0xbc91,0xbc93,0xbc95,0xbc96,0xbc97,
   0xbca1,0xbca5,0xbcb7,0xbce1,0xbce2,0xbce5,0xbce9,0xbcf1,
   0xbcf3,0xbcf5,0xbcf6,0xbcf7,0xbd41,0xbd57,0xbd61,0xbd76,
   0xbda1,0xbda2,0xbda5,0xbda9,0xbdb1,0xbdb3,0xbdb5,0xbdb7,
   0xbdb9,0xbdc1,0xbdc2,0xbdc9,0xbdd6,0xbde1,0xbdf6,0xbe41,
   0xbe45,0xbe49,0xbe51,0xbe53,0xbe77,0xbe81,0xbe82,0xbe85,
   0xbe89,0xbe91,0xbe93,0xbe97,0xbea1,0xbeb6,0xbeb7,0xbee1,
   0xbf41,0xbf61,0xbf71,0xbf75,0xbf77,0xbfa1,0xbfa2,0xbfa5,
   0xbfa9,0xbfb1,0xbfb3,0xbfb7,0xbfb8,0xbfbd,0xc061,0xc062,
   0xc065,0xc067,0xc069,0xc071,0xc073,0xc075,

	/*
	0xC3A1 - 0xC3FE
	   찼 창 찾 채 책 챈 챌 챔 챕 챗 챘 챙 챠 챤 챦 
	챨 챰 챵 처 척 천 철 첨 첩 첫 첬 청 체 첵 첸 첼 
	쳄 쳅 쳇 쳉 쳐 쳔 쳤 쳬 쳰 촁 초 촉 촌 촐 촘 촙 
	촛 총 촤 촨 촬 촹 최 쵠 쵤 쵬 쵭 쵯 쵱 쵸 춈 추 
	축 춘 출 춤 춥 춧 충 춰 췄 췌 췐 취 췬 췰 췸 췹 
	췻 췽 츄 츈 츌 츔 츙 츠 측 츤 츨 츰 츱 츳 층 
	*/
   
   0xc076,0xc077,0xc078,0xc081,0xc082,0xc085,0xc089,0xc091,
   0xc093,0xc095,0xc096,0xc097,0xc0a1,0xc0a5,0xc0a7,0xc0a9,
   0xc0b1,0xc0b7,0xc0e1,0xc0e2,0xc0e5,0xc0e9,0xc0f1,0xc0f3,
   0xc0f5,0xc0f6,0xc0f7,0xc141,0xc142,0xc145,0xc149,0xc151,
   0xc153,0xc155,0xc157,0xc161,0xc165,0xc176,0xc181,0xc185,
   0xc197,0xc1a1,0xc1a2,0xc1a5,0xc1a9,0xc1b1,0xc1b3,0xc1b5,
   0xc1b7,0xc1c1,0xc1c5,0xc1c9,0xc1d7,0xc241,0xc245,0xc249,
   0xc251,0xc253,0xc255,0xc257,0xc261,0xc271,0xc281,0xc282,
   0xc285,0xc289,0xc291,0xc293,0xc295,0xc297,0xc2a1,0xc2b6,
   0xc2c1,0xc2c5,0xc2e1,0xc2e5,0xc2e9,0xc2f1,0xc2f3,0xc2f5,
   0xc2f7,0xc341,0xc345,0xc349,0xc351,0xc357,0xc361,0xc362,
   0xc365,0xc369,0xc371,0xc373,0xc375,0xc377,

	/*
	0xC4A1 - 0xC4FE
	   치 칙 친 칟 칠 칡 침 칩 칫 칭 카 칵 칸 칼 캄 
	캅 캇 캉 캐 캑 캔 캘 캠 캡 캣 캤 캥 캬 캭 컁 커 
	컥 컨 컫 컬 컴 컵 컷 컸 컹 케 켁 켄 켈 켐 켑 켓 
	켕 켜 켠 켤 켬 켭 켯 켰 켱 켸 코 콕 콘 콜 콤 콥 
	콧 콩 콰 콱 콴 콸 쾀 쾅 쾌 쾡 쾨 쾰 쿄 쿠 쿡 쿤 
	쿨 쿰 쿱 쿳 쿵 쿼 퀀 퀄 퀑 퀘 퀭 퀴 퀵 퀸 퀼 
	*/
   
   0xc3a1,0xc3a2,0xc3a5,0xc3a8,0xc3a9,0xc3aa,0xc3b1,0xc3b3,
   0xc3b5,0xc3b7,0xc461,0xc462,0xc465,0xc469,0xc471,0xc473,
   0xc475,0xc477,0xc481,0xc482,0xc485,0xc489,0xc491,0xc493,
   0xc495,0xc496,0xc497,0xc4a1,0xc4a2,0xc4b7,0xc4e1,0xc4e2,
   0xc4e5,0xc4e8,0xc4e9,0xc4f1,0xc4f3,0xc4f5,0xc4f6,0xc4f7,
   0xc541,0xc542,0xc545,0xc549,0xc551,0xc553,0xc555,0xc557,
   0xc561,0xc565,0xc569,0xc571,0xc573,0xc575,0xc576,0xc577,
   0xc581,0xc5a1,0xc5a2,0xc5a5,0xc5a9,0xc5b1,0xc5b3,0xc5b5,
   0xc5b7,0xc5c1,0xc5c2,0xc5c5,0xc5c9,0xc5d1,0xc5d7,0xc5e1,
   0xc5f7,0xc641,0xc649,0xc661,0xc681,0xc682,0xc685,0xc689,
   0xc691,0xc693,0xc695,0xc697,0xc6a1,0xc6a5,0xc6a9,0xc6b7,
   0xc6c1,0xc6d7,0xc6e1,0xc6e2,0xc6e5,0xc6e9,

	/*
	0xC5A1 - 0xC5FE
	   큄 큅 큇 큉 큐 큔 큘 큠 크 큭 큰 클 큼 큽 킁 
	키 킥 킨 킬 킴 킵 킷 킹 타 탁 탄 탈 탉 탐 탑 탓 
	탔 탕 태 택 탠 탤 탬 탭 탯 탰 탱 탸 턍 터 턱 턴 
	털 턺 텀 텁 텃 텄 텅 테 텍 텐 텔 템 텝 텟 텡 텨 
	텬 텼 톄 톈 토 톡 톤 톨 톰 톱 톳 통 톺 톼 퇀 퇘 
	퇴 퇸 툇 툉 툐 투 툭 툰 툴 툼 툽 툿 퉁 퉈 퉜 
	*/
   
   0xc6f1,0xc6f3,0xc6f5,0xc6f7,0xc741,0xc745,0xc749,0xc751,
   0xc761,0xc762,0xc765,0xc769,0xc771,0xc773,0xc777,0xc7a1,
   0xc7a2,0xc7a5,0xc7a9,0xc7b1,0xc7b3,0xc7b5,0xc7b7,0xc861,
   0xc862,0xc865,0xc869,0xc86a,0xc871,0xc873,0xc875,0xc876,
   0xc877,0xc881,0xc882,0xc885,0xc889,0xc891,0xc893,0xc895,
   0xc896,0xc897,0xc8a1,0xc8b7,0xc8e1,0xc8e2,0xc8e5,0xc8e9,
   0xc8eb,0xc8f1,0xc8f3,0xc8f5,0xc8f6,0xc8f7,0xc941,0xc942,
   0xc945,0xc949,0xc951,0xc953,0xc955,0xc957,0xc961,0xc965,
   0xc976,0xc981,0xc985,0xc9a1,0xc9a2,0xc9a5,0xc9a9,0xc9b1,
   0xc9b3,0xc9b5,0xc9b7,0xc9bc,0xc9c1,0xc9c5,0xc9e1,0xca41,
   0xca45,0xca55,0xca57,0xca61,0xca81,0xca82,0xca85,0xca89,
   0xca91,0xca93,0xca95,0xca97,0xcaa1,0xcab6,

	/*
	0xC6A1 - 0xC6FE
	퉤 튀 튁 튄 튈 튐 튑 튕 튜 튠 튤 튬 튱 트 특 
	튼 튿 틀 틂 틈 틉 틋 틔 틘 틜 틤 틥 티 틱 틴 틸	
	팀 팁 팃 팅 파 팍 팎 판 팔 팖 팜 팝 팟 팠 팡 팥 
	패 팩 팬 팰 팸 팹 팻 팼 팽 퍄 퍅 퍼 퍽 펀 펄 펌 
	펍 펏 펐 펑 페 펙 펜 펠 펨 펩 펫 펭 펴 편 펼 폄 
	폅 폈 평 폐 폘 폡 폣 포 폭 폰 폴 폼 폽 폿 퐁 
	*/
   
   0xcac1,0xcae1,0xcae2,0xcae5,0xcae9,0xcaf1,0xcaf3,0xcaf7,
   0xcb41,0xcb45,0xcb49,0xcb51,0xcb57,0xcb61,0xcb62,0xcb65,
   0xcb68,0xcb69,0xcb6b,0xcb71,0xcb73,0xcb75,0xcb81,0xcb85,
   0xcb89,0xcb91,0xcb93,0xcba1,0xcba2,0xcba5,0xcba9,0xcbb1,
   0xcbb3,0xcbb5,0xcbb7,0xcc61,0xcc62,0xcc63,0xcc65,0xcc69,
   0xcc6b,0xcc71,0xcc73,0xcc75,0xcc76,0xcc77,0xcc7b,0xcc81,
   0xcc82,0xcc85,0xcc89,0xcc91,0xcc93,0xcc95,0xcc96,0xcc97,
   0xcca1,0xcca2,0xcce1,0xcce2,0xcce5,0xcce9,0xccf1,0xccf3,
   0xccf5,0xccf6,0xccf7,0xcd41,0xcd42,0xcd45,0xcd49,0xcd51,
   0xcd53,0xcd55,0xcd57,0xcd61,0xcd65,0xcd69,0xcd71,0xcd73,
   0xcd76,0xcd77,0xcd81,0xcd89,0xcd93,0xcd95,0xcda1,0xcda2,
   0xcda5,0xcda9,0xcdb1,0xcdb3,0xcdb5,0xcdb7,

	/*
	0xC7A1 - 0xC7FE
	   퐈 퐝 푀 푄 표 푠 푤 푭 푯 푸 푹 푼 푿 풀 풂 
	품 풉 풋 풍 풔 풩 퓌 퓐 퓔 퓜 퓟 퓨 퓬 퓰 퓸 퓻 
	퓽 프 픈 플 픔 픕 픗 피 픽 핀 필 핌 핍 핏 핑 하 
	학 한 할 핥 함 합 핫 항 해 핵 핸 핼 햄 햅 햇 했 
	행 햐 향 허 헉 헌 헐 헒 험 헙 헛 헝 헤 헥 헨 헬 
	헴 헵 헷 헹 혀 혁 현 혈 혐 협 혓 혔 형 혜 혠 
	*/
   
   0xcdc1,0xcdd7,0xce41,0xce45,0xce61,0xce65,0xce69,0xce73,
   0xce75,0xce81,0xce82,0xce85,0xce88,0xce89,0xce8b,0xce91,
   0xce93,0xce95,0xce97,0xcea1,0xceb7,0xcee1,0xcee5,0xcee9,
   0xcef1,0xcef5,0xcf41,0xcf45,0xcf49,0xcf51,0xcf55,0xcf57,
   0xcf61,0xcf65,0xcf69,0xcf71,0xcf73,0xcf75,0xcfa1,0xcfa2,
   0xcfa5,0xcfa9,0xcfb1,0xcfb3,0xcfb5,0xcfb7,0xd061,0xd062,
   0xd065,0xd069,0xd06e,0xd071,0xd073,0xd075,0xd077,0xd081,
   0xd082,0xd085,0xd089,0xd091,0xd093,0xd095,0xd096,0xd097,
   0xd0a1,0xd0b7,0xd0e1,0xd0e2,0xd0e5,0xd0e9,0xd0eb,0xd0f1,
   0xd0f3,0xd0f5,0xd0f7,0xd141,0xd142,0xd145,0xd149,0xd151,
   0xd153,0xd155,0xd157,0xd161,0xd162,0xd165,0xd169,0xd171,
   0xd173,0xd175,0xd176,0xd177,0xd181,0xd185,

	/*
	0xC8A1 - 0xC8FE
	   혤 혭 호 혹 혼 홀 홅 홈 홉 홋 홍 홑 화 확 환 
	활 홧 황 홰 홱 홴 횃 횅 회 획 횐 횔 횝 횟 횡 효 
	횬 횰 횹 횻 후 훅 훈 훌 훑 훔 훗 훙 훠 훤 훨 훰 
	훵 훼 훽 휀 휄 휑 휘 휙 휜 휠 휨 휩 휫 휭 휴 휵 
	휸 휼 흄 ?흉 흐 흑 흔 흖 흗 흘 흙 흠 흡 흣 흥 
	흩 희 흰 흴 흼 흽 힁 히 힉 힌 힐 힘 힙 힛 힝 
	*/
   
   0xd189,0xd193,0xd1a1,0xd1a2,0xd1a5,0xd1a9,0xd1ae,0xd1b1,
   0xd1b3,0xd1b5,0xd1b7,0xd1bb,0xd1c1,0xd1c2,0xd1c5,0xd1c9,
   0xd1d5,0xd1d7,0xd1e1,0xd1e2,0xd1e5,0xd1f5,0xd1f7,0xd241,
   0xd242,0xd245,0xd249,0xd253,0xd255,0xd257,0xd261,0xd265,
   0xd269,0xd273,0xd275,0xd281,0xd282,0xd285,0xd289,0xd28e,
   0xd291,0xd295,0xd297,0xd2a1,0xd2a5,0xd2a9,0xd2b1,0xd2b7,
   0xd2c1,0xd2c2,0xd2c5,0xd2c9,0xd2d7,0xd2e1,0xd2e2,0xd2e5,
   0xd2e9,0xd2f1,0xd2f3,0xd2f5,0xd2f7,0xd341,0xd342,0xd345,
   0xd349,0xd351,0xd355,0xd357,0xd361,0xd362,0xd365,0xd367,
   0xd368,0xd369,0xd36a,0xd371,0xd373,0xd375,0xd377,0xd37b,
   0xd381,0xd385,0xd389,0xd391,0xd393,0xd397,0xd3a1,0xd3a2,
   0xd3a5,0xd3a9,0xd3b1,0xd3b3,0xd3b5,0xd3b7
};
