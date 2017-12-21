		if ( strcmp(function_name,"%FUNC_NAME%")==0 )
		{
			%ARG_DECLARE(;)%;
			%NO_ARG%in >> %ARG_LIST(>>)%;
			const %RETURN_TYPE% & vReturn= m_real_service->%FUNC_NAME%(%ARG_LIST(,)%);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}