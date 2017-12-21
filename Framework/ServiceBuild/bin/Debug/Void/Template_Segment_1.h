   //////////////////////////////////////////////////////////////////////////////////
   virtual %RETURN_TYPE%  %FUNC_NAME%( %ARG_DECLARE(,)% )
    {
	    obuf256 t_data;
	    t_data << "%FUNC_NAME%";
	    %NO_ARG%t_data << %ARG_LIST(<<)%;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen,MSG_FLAG_NO_BLOCK);
    };
