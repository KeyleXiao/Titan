-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	���к�
-- ��  ��:	2017/1/18
-- ��  ��:	1.0
-- ��  ��:	NPC������Ϣ���ñ�
-------------------------------------------------------------------

-- ����NPC����Ч����
NPC_TRIGGER_DISTANCE = 10

-- ���NPC����
WINGS_CONFIG = {
	["cost"] = 100, 			-- �����츳����
	["buff_id"] = 2377, 		-- ���buff ID
	["no_enough_gold"] = ("��Ľ�Ҳ���100���ܷ���"), 
}

	-- id:װ��id��cool_time:�´ζ���Ժ�ſ��Թ���refresh_time:��ò���һ��������Ϊ0��û����ȴʱ�䲻�Ჹ������;
	-- can_buy_count:���ɹ����������������Ժ�װ��ͼ���ɻ�ɫ
	-- after_buy_disappear��������ɺ�NPC��ʧ
	-- name:��Ʒ����
	-- goods_cost����Ʒ�۸�
	-- min_value����С���ֵ
	-- max_value��������ֵ
GOODSSHOP_CONFIG = {
	[2178] = {
		min_random = 1, max_random = 10000, random_count = 6, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 277, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���ܷ���", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[2] = {id = 286, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $ǿϮ��", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[3] = {id = 291, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����쫷�", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[4] = {id = 285, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��������", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[5] = {id = 257, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���������", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[6] = {id = 258, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��������", goods_cost =-1, min_value =1, max_value = 10000,}, 
		}
	}, 

	[2179] = {
		min_random = 1, max_random = 10000, random_count = 9, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 286, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $ǿϮ��", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[2] = {id = 285, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��������", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[3] = {id = 258, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��������", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[4] = {id = 259, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $ʥ��þ�", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[5] = {id = 278, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��������", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[6] = {id = 292, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $Ѫ����", goods_cost =-1, min_value =1, max_value = 10000,}, 
		}
	}, 
			
	[2181] = {
		min_random = 1, max_random = 10000, random_count = 18, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 280, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���̺ڹ�", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[2] = {id = 279, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[3] = {id = 261, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����ħ��", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[4] = {id = 277, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���ܷ���", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[5] = {id = 291, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����쫷�", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[6] = {id = 257, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���������", goods_cost =-1, min_value =1, max_value = 10000,}, 
		}
	}, 
			
	[2182] = {
		min_random = 1, max_random = 10000, random_count = 7, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 254, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $֧�������", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[2] = {id = 266, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����֮��", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[3] = {id = 289, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����֮��", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[4] = {id = 286, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $ǿϮ��", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[5] = {id = 285, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��������", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[6] = {id = 258, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��������", goods_cost =-1, min_value =1, max_value = 10000,}, 
		}
	}, 
	
	[2183] = {
		min_random = 1, max_random = 10000, random_count = 7, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 277, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���ܷ���", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[2] = {id = 291, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����쫷�", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[3] = {id = 257, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���������", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[4] = {id = 282, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $ն�ѽ�", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[5] = {id = 288, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����̫��", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[6] = {id = 293, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���Ͻ����", goods_cost =-1, min_value =1, max_value = 10000,}, 
		}
	}, 
	
	[2184] = {
		min_random = 1, max_random = 10000, random_count = 6, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 280, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���̺ڹ�", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[2] = {id = 279, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[3] = {id = 261, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����ħ��", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[4] = {id = 259, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $ʥ��þ�", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[5] = {id = 278, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��������", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[6] = {id = 292, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $Ѫ����", goods_cost =-1, min_value =1, max_value = 10000,}, 
		}
	}, 
	
	[2185] = {
		min_random = 1, max_random = 10000, random_count = 6, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 254, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $֧�������", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[2] = {id = 266, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����֮��", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[3] = {id = 289, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����֮��", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[4] = {id = 282, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $ն�ѽ�", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[5] = {id = 288, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����̫��", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[6] = {id = 293, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���Ͻ����", goods_cost =-1, min_value =1, max_value = 10000,}, 
		}
	},
	
    [2186] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 245, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "��������(����)", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
	[2187] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 246, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "��֮ѥ��������", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
	[2188] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 247, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "ն�ѽ�(����)", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
    [2189] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 248, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "����������(����)", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
	[2190] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		[1] = {id = 249, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "���Ͻ����(����)", goods_cost = 0, min_value = 1, max_value = 10000}
	}, 
	
	[2191] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 250, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "Ů�����(����)", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
	[2192] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 251, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "��ʦñ��(����)", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
	[2193] = {
		min_random = 1, max_random = 10000, random_count = 4, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 252, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "�ݶ�����(����)", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
	[2194] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 253, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "����ħ��(����)", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
	[2195] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 254, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "֧�������(����)", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
	--��ϡ�д��
	[2205] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = true, 
		goods_table = {
			[1] = {id = 245, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��������(����)", goods_cost =0, min_value =1, max_value = 1000,}, 
			[2] = {id = 246, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��֮ѥ��������", goods_cost =0, min_value =1001, max_value = 2000,}, 
			[3] = {id = 247, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $ն�ѽ�(����)", goods_cost =0, min_value =2001, max_value = 3000,}, 
			[4] = {id = 248, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����������(����)", goods_cost =0, min_value =3001, max_value = 4000,}, 
			[5] = {id = 249, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���Ͻ����(����)", goods_cost =0, min_value =4001, max_value = 5000,}, 
			[6] = {id = 280, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���̺ڹ�", goods_cost =0, min_value =5001, max_value = 6000,}, 
			[7] = {id = 279, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����", goods_cost =0, min_value =6001, max_value = 7000,}, 
			[8] = {id = 252, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $�ݶ�����(����)", goods_cost =0, min_value =7001, max_value = 8000,}, 
			[9] = {id = 253, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����ħ��(����)", goods_cost =0, min_value =8001, max_value = 9000,}, 
			[10] = {id = 254, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $֧�������", goods_cost =0, min_value =9001, max_value = 10000,}, 
		}
	}, 
	
	--����ͨ������
	[2206] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = true, 
		goods_table = {
			[1] = {id = 277, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���ܷ���", goods_cost =0, min_value =1, max_value = 1000,}, 
			[2] = {id = 278, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��������", goods_cost =0, min_value =1001, max_value = 2000,}, 
			[3] = {id = 289, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����֮��", goods_cost =0, min_value =2001, max_value = 3000,}, 
			[4] = {id = 286, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $ǿϮ��", goods_cost =0, min_value =3001, max_value = 4000,}, 
			[5] = {id = 282, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $ն�ѽ�", goods_cost =0, min_value =4001, max_value = 5000,}, 
			[6] = {id = 291, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����쫷�", goods_cost =0, min_value =5001, max_value = 6000,}, 
			[7] = {id = 285, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��������", goods_cost =0, min_value =6001, max_value = 7000,}, 
			[8] = {id = 288, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����̫��", goods_cost =0, min_value =7001, max_value = 8000,}, 
			[9] = {id = 293, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���Ͻ����", goods_cost =0, min_value =8001, max_value = 9000,}, 
			[10] = {id = 292, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $Ѫ����", goods_cost =0, min_value =9001, max_value = 10000,}, 
		}
	}, 
	--��������С��
	[2207] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = true, 
		goods_table = {
			[1] = {id = 306, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��ħȭ��", goods_cost =0, min_value =1, max_value = 1000,}, 
			[2] = {id = 308, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����̹�", goods_cost =0, min_value =1001, max_value = 2000,}, 
			[3] = {id = 304, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $׷��", goods_cost =0, min_value =2001, max_value = 3000,}, 
			[4] = {id = 299, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��ʿ����", goods_cost =0, min_value =3001, max_value = 4000,}, 
			[5] = {id = 309, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���ذ��", goods_cost =0, min_value =4001, max_value = 5000,}, 
			[6] = {id = 310, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����", goods_cost =0, min_value =5001, max_value = 6000,}, 
			[7] = {id = 305, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $ˮ����", goods_cost =0, min_value =6001, max_value = 7000,}, 
			[8] = {id = 302, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��������", goods_cost =0, min_value =7001, max_value = 8000,}, 
			[9] = {id = 300, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���ִ�", goods_cost =0, min_value =8001, max_value = 9000,}, 
			[10] = {id = 301, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $�̿ͷɵ�", goods_cost =0, min_value =9001, max_value = 10000,}, 
		}
	}, 
	--���з���С��
	[2208] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = true, 
		goods_table = {
			[1] = {id = 257, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���������", goods_cost =0, min_value =1, max_value = 2000,}, 
			[2] = {id = 258, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��������", goods_cost =0, min_value =2001, max_value = 4000,}, 
			[3] = {id = 259, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $ʥ��þ�", goods_cost =0, min_value =4001, max_value = 6000,}, 
			[4] = {id = 266, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����֮��", goods_cost =0, min_value =6001, max_value = 8000,}, 
			[5] = {id = 261, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����ħ��", goods_cost =0, min_value =8001, max_value = 10000,}, 
		}
	},
	--���з���С��
	[2209] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = true, 
		goods_table = {
			[1] = {id = 270, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $ʥͽ����", goods_cost =0, min_value =1, max_value = 2000,}, 
			[2] = {id = 274, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $����֮��", goods_cost =0, min_value =2001, max_value = 4000,}, 
			[3] = {id = 269, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $��ˮ��", goods_cost =0, min_value =4001, max_value = 6000,}, 
			[4] = {id = 275, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $ħ��", goods_cost =0, min_value =6001, max_value = 8000,}, 
			[5] = {id = 268, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $���ߵ伮", goods_cost =0, min_value =8001, max_value = 10000,}, 
		}
	},
}


