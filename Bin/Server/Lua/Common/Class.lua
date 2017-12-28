-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	���� (Carl Chen)
-- ��  ��:	04/25/2008
-- ��  ��:	1.0
-- ��  ��:	����lua���е��﷨,ʵ�ּ򵥵���̳�
-------------------------------------------------------------------


-- ��������
Object = {}

-- ��Ա������ʽ��new 
function Object:new(o)
	o = o or {}

	-- �̳и���ķ���
	if o.parent==nil then
    	-- Table������Ĭ�������õ�,������Ҫ�ӻ��࿽��һ��,��������׳���Ī������Ĵ���
		for key,value in pairs(self) do
			if type(value)=='table' and key~='parent' then
				if o[key]==nil or o[key]==value then
		      		o[key]={}
			  		copy_table(o[key],value)
		   		end
			end
		end
	    
		o.parent = self

		-- �̳и��෽��
		setmetatable(o,{__index = self})
	end
	
	-- ������new
	if self.parent~=nil then
		o = self.parent:new(o)
	end

	-- �����캯��
	if self.construct then
		self.construct(o)
	end

	return o
end

-- ��Ա������ʽ��delete
function Object:delete()
-- ������delete	
	local parent = self
	for i=1,64,1 do 
		-- ���͹����� 
		if parent.destroy then
    			parent.destroy(self)
		end
	
		parent = parent.parent  
		if parent==nil then
			break
		end	
	end
end

-- ��������ʽ��new 
function new(type,param)
    param = param or {}
    return type:new(param)
end
 
-- ��������ʽ��delete
function delete(obj)
    obj:delete()
    obj = nil
end
