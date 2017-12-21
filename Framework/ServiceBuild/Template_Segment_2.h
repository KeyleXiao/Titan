		if ( StringHelper::casecmp(function_name,"%FUNC_NAME%")==0 )
		{
			IServiceContainer * pService = 0;
			%ARG_DECLARE(;)%;
			%NO_ARG%in >> %ARG_LIST(>>)%;
			%RETURN_TYPE% vReturn= m_real_service->%FUNC_NAME%(%ARG_LIST(,)%);
			pResult = &vReturn;
			nResultLen = sizeof(vReturn);
			return true;
		}