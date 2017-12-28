-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	陈涛 (Carl Chen)
-- 日  期:	04/25/2008
-- 版  本:	1.0
-- 描  述:	利用lua现有的语法,实现简单的类继承
-------------------------------------------------------------------


-- 公共基类
Object = {}

-- 成员函数形式的new 
function Object:new(o)
	o = o or {}

	-- 继承父类的方法
	if o.parent==nil then
    	-- Table型数据默认是引用的,所以需要从基类拷贝一份,否则很容易出现莫名奇妙的错误
		for key,value in pairs(self) do
			if type(value)=='table' and key~='parent' then
				if o[key]==nil or o[key]==value then
		      		o[key]={}
			  		copy_table(o[key],value)
		   		end
			end
		end
	    
		o.parent = self

		-- 继承父类方法
		setmetatable(o,{__index = self})
	end
	
	-- 调父类new
	if self.parent~=nil then
		o = self.parent:new(o)
	end

	-- 调构造函数
	if self.construct then
		self.construct(o)
	end

	return o
end

-- 成员函数形式的delete
function Object:delete()
-- 调父类delete	
	local parent = self
	for i=1,64,1 do 
		-- 调释构函数 
		if parent.destroy then
    			parent.destroy(self)
		end
	
		parent = parent.parent  
		if parent==nil then
			break
		end	
	end
end

-- 操作符形式的new 
function new(type,param)
    param = param or {}
    return type:new(param)
end
 
-- 操作符形式的delete
function delete(obj)
    obj:delete()
    obj = nil
end
