       virtual %RETURN_TYPE%  %FUNC_NAME%( %ARG_DECLARE(,)% )
        {
	        obuf256 data;
	        data << "%FUNC_NAME%";
	        %NO_ARG%data << %ARG_LIST(<<)%;

	        void * pReturn = 0;
	        int nReturnLen = 0;
	        m_pContainer->post_message( data.data(),data.size(),pReturn,nReturnLen);
	        if ( nReturnLen==sizeof(%RETURN_TYPE%) && pReturn!=0)
	        {
		        return *(%RETURN_TYPE%*)pReturn;
	        }

		return %RETURN_TYPE%()
        };