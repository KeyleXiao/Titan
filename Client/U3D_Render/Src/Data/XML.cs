using UnityEngine;
using System.Collections;
using System;  
using System.Xml;  
using System.Xml.Serialization;  
using System.IO;  
using System.Text;  
  
//约束条件  
public class TestData  
{  
	public String server ;
   	public String account ;
   	public String password;
}  
  
//数组约束  
public class UserData  
{  
    public TestData _iUser  = new TestData();  
    //function UserData() { }  
}  
  public class XML {


private String c1 ;
private String c2 ;  
private String _FileLocation ;  
private String _FileName  = "Data.xml";  
private String _data;  
public UserData myData;  
public UserData tempData  = new UserData();  
  
private int i  = 0;  
  
public void startXml(){
		_FileLocation = Application.persistentDataPath;  //文件位置
		//_FileLocation = Application.dataPath;
		Trace.Log(_FileLocation);
  
   		FirstSave();  
	}
    

  
//实现一个 TextWriter，使其以一种特定的编码向流中写入字符。   
StreamWriter streamWriter ;  
  
public void FirstSave()  
{//初始化XML  
	tempData._iUser.server="";
    tempData._iUser.account = "";  
    tempData._iUser.password =	""; 
    //FileInfo 方法在您创建或打开文件时返回其他 I/O 类型  
    FileInfo fileInfo  = new FileInfo(_FileLocation+"/"+ _FileName);  
//    print(fileInfo.Exists);
    if(!fileInfo.Exists)  
    {  

        streamWriter = fileInfo.CreateText();  
         
        //XML化  
        _data = SerializeObject(tempData);  
        for(i=0;i<1;i++)  
        {  
            streamWriter.WriteLine(_data);  
        }  
          
        streamWriter.Close();  
    }  
    else  
    {  
        
    }  
}  
  
 

public void Save(String sc1 , String sc2 ,String sc3)
{//保存数据到指定的XMl里  
	tempData._iUser.server 		= sc1;  
    tempData._iUser.account 	= sc2;   
	tempData._iUser.password 	= sc3;  
    //FileInfo 方法在您创建或打开文件时返回其他 I/O 类型  
    FileInfo fileInfo 			= new FileInfo(_FileLocation+"/"+ _FileName);  
    StreamWriter streamWriter;  
    //fileInfo.Delete();  
    streamWriter				= fileInfo.CreateText();  
    _data = SerializeObject(tempData);  
    for(i=0;i<1;i++)  
    {  
        streamWriter.WriteLine(_data);  
    }  
    streamWriter.Close();  
}
  
  
  
public void Load()  
{//读取保存在XML里的数据  
    StreamReader streamReader  		= File.OpenText(_FileLocation+"/"+ _FileName);  
      
    for(i=0;i<1;i++)  
    {  
        _data = streamReader.ReadLine();  
        myData = (UserData)DeserializeObject(_data);  
    }  
      
    streamReader.Close();  
}  
  
    
  
public String UTF8ByteArrayToString(byte []characters)  
{  
        UTF8Encoding encoding 		= new UTF8Encoding();  
        String constructedString 	= encoding.GetString(characters);  
        return (constructedString);  
 }  
   
 public byte[] StringToUTF8ByteArray(String pXmlString)  
 {  
         UTF8Encoding encoding 		= new UTF8Encoding();  
        byte []byteArray = encoding.GetBytes(pXmlString);  
        return byteArray;  
 }  
  
 // Here we serialize our UserData object of myData  
 public String SerializeObject(object pObject)  
 {  
        String XmlizedString 		= "";  
        MemoryStream memoryStream 	= new MemoryStream();  
        XmlSerializer xs 			= new XmlSerializer(typeof(UserData));  
        XmlTextWriter xmlTextWriter = new XmlTextWriter(memoryStream, Encoding.UTF8);  
        xs.Serialize(xmlTextWriter, pObject);  
        memoryStream 				= (MemoryStream)xmlTextWriter.BaseStream; // (MemoryStream)  
        XmlizedString 				= UTF8ByteArrayToString(memoryStream.ToArray());  
        return XmlizedString;  
 }  
  
 // Here we deserialize it back into its original form  
public object DeserializeObject(String pXmlizedString)  
 {  
        XmlSerializer xs   			= new XmlSerializer(typeof(UserData));  
        MemoryStream memoryStream 	= new MemoryStream(StringToUTF8ByteArray(pXmlizedString)); 
        return xs.Deserialize(memoryStream);  
 }   
}
