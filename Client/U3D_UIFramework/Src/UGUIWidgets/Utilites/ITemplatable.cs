/*******************************************************************
** 文件名:	ITemplatable.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-19
** 版  本:	1.8.0
** 描  述:	
		
********************************************************************/


namespace UIWidgets {
	/// <summary>
	/// ITemplatable interface.
	/// </summary>
	public interface ITemplatable {
		/// <summary>
		/// Gets a value indicating whether this instance is template.
		/// </summary>
		/// <value><c>true</c> if this instance is template; otherwise, <c>false</c>.</value>
		bool IsTemplate {
			get;
			set;
		}
		
		/// <summary>
		/// Gets the name of the template.
		/// </summary>
		/// <value>The name of the template.</value>
		string TemplateName {
			get;
			set;
		}
	}
}
