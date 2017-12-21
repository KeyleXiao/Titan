   //////////////////////////////////////////////////////////////////////////////////
   virtual %RETURN_TYPE%  %FUNC_NAME%( %ARG_DECLARE(,)% )
    {
	    obuf256 t_data;
	    t_data << "%FUNC_NAME%";
	    %NO_ARG%t_data << %ARG_LIST(<<)%;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    %RETURN_TYPE% vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return %RETURN_TYPE%();     // 请检查默认返回值是否正确
    };
