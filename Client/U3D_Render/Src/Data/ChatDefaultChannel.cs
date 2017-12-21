using System;
using System.Collections.Generic;
using UnityEngine;

namespace ASpeedGame.Data.ChatDefaultChannel
{
    public enum EChatDefaultChannelCol : int
    {
        COL_MatchType = 0,
        COL_ChannelInRoom   // 房间默认发言频道
    }

    public class SChatDefaultChannelScheme
    {
        public int nMatchType;
        public int nChannelInRoom;   // 房间默认发言频道
    }


    public class ChatDefaultChannel
    {
        public Dictionary<int, SChatDefaultChannelScheme> configScheme;
        private static ChatDefaultChannel singleton = new ChatDefaultChannel();

        public static ChatDefaultChannel Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new ChatDefaultChannel();
            }
        }

        public void Load(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            configScheme = new Dictionary<int, SChatDefaultChannelScheme>();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SChatDefaultChannelScheme scheme = new SChatDefaultChannelScheme();
                scheme.nMatchType = reader.GetInt(i, (int)EChatDefaultChannelCol.COL_MatchType, 0);
                scheme.nChannelInRoom = reader.GetInt(i, (int)EChatDefaultChannelCol.COL_ChannelInRoom, 0);
                configScheme[scheme.nMatchType] = scheme;
            }
        }

        public void UnLoad()
        {
            if(configScheme != null)
            {
                configScheme.Clear();
            }
            singleton = null;
        }

        public int getDefaultChannelInRoom(int nMatchType)
        {
            if(!configScheme.ContainsKey(nMatchType))
            {
                Debug.LogError("There isn't contains the key, nMatchType=" + nMatchType);
                return -1;
            }

            return configScheme[nMatchType].nChannelInRoom;
        }
    }
}
