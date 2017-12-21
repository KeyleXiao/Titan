using UnityEngine;
using System.Collections;
using System.Text.RegularExpressions;
using RichTextPart;

public interface URichTextHandlerInterface{

    string HandleText(ref int addTextCount, string text, int startIndex, int endIndex, int length, GroupCollection group);

    bool HandleTextMesh(URichTextParseInfo info);

    bool IsSingleRegex();
    int GetRegexType();
    string[] GetRegexInfo();
}
