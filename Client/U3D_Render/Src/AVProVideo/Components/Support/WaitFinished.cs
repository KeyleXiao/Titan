//#if PLAYMAKER
//using UnityEngine;
//using HutongGames.PlayMaker.Actions;
//using RenderHeads.Media.AVProVideo;

////-----------------------------------------------------------------------------
//// Copyright 2015-2016 RenderHeads Ltd.  All rights reserverd.
////-----------------------------------------------------------------------------

//namespace RenderHeads.Media.AVProVideo.PlayMaker.Actions
//{
//    [ActionCategory(ActionCategory.Time)]
//    [Tooltip("Delays a State from finishing until the media has finished playing.")]
//    public class WaitFinished : FsmStateAction
//    {
//        [RequiredField]
//        public MediaPlayer mediaPlayer;

//        public FsmEvent finishEvent;

//        public override void Reset()
//        {
//            finishEvent = null;
//        }

//        public override void OnEnter()
//        {
//            if (mediaPlayer.Control == null)
//            {
//                if (finishEvent != null)
//                {
//                    Fsm.Event(finishEvent);
//                }
//                Finish();
//            }
//        }

//        public override void OnUpdate()
//        {
//            if (mediaPlayer.Control.IsFinished())
//            {
//                Finish();
//                if (finishEvent != null)
//                {
//                    Fsm.Event(finishEvent);
//                }
//            }
//        }

//    }
//}
//#endif
