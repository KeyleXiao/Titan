		if ( strcmp(function_name,"%FUNC_NAME%")==0 )
		{
			%ARG_DECLARE(;)%;
			%NO_ARG%in >> %ARG_LIST(>>)%;
			m_real_service->%FUNC_NAME%(%ARG_LIST(,)%);
			return true;
		}
