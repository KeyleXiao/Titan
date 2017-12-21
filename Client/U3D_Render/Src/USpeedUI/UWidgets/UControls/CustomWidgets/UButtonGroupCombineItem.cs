using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;


public class UButtonGroupCombineItem : UButtonGroupItem
{
    public Image[] ChangeStateItems; 
    public Sprite[] NormalSprites;
    public Sprite[] HighlightedSprites;
    public Sprite[] PressedSprites;
    public Sprite[] DisabledSprites;

    protected override void DoStateTransition(SelectionState state, bool instant)
    {
        if (Toggle && Selected)
        {
            ChangeState(SelectionState.Pressed);
            return;
        }
        else if (Toggle && !Selected)
        {
            ChangeState(SelectionState.Normal);
            return;
        }

        ChangeState(state);
    }

    private void ChangeState(SelectionState state)
    {
        if (ChangeStateItems == null || ChangeStateItems.Length <= 0)
        {
            return;
        }
        switch (state)
        {
            case SelectionState.Disabled:
                {
                    if (DisabledSprites != null || DisabledSprites.Length > 0)
                    {
                        for (int i = 0; i < ChangeStateItems.Length && i < DisabledSprites.Length; i++)
                        {
                            ChangeStateItems[i].sprite = DisabledSprites[i];
                        }
                    }
                }
                break;
            case SelectionState.Highlighted:
                if (HighlightedSprites != null || HighlightedSprites.Length > 0)
                {
                    for (int i = 0; i < ChangeStateItems.Length && i < HighlightedSprites.Length; i++)
                    {
                        ChangeStateItems[i].sprite = HighlightedSprites[i];
                    }
                }
                break;
            case SelectionState.Normal:
                if (NormalSprites != null || NormalSprites.Length > 0)
                {
                    for (int i = 0; i < ChangeStateItems.Length && i < NormalSprites.Length; i++)
                    {
                        ChangeStateItems[i].sprite = NormalSprites[i];
                    }
                }
                break;
            case SelectionState.Pressed:
                if (PressedSprites != null || PressedSprites.Length > 0)
                {
                    for (int i = 0; i < ChangeStateItems.Length && i < PressedSprites.Length; i++)
                    {
                        ChangeStateItems[i].sprite = PressedSprites[i];
                    }
                }
                break;
            default:
                break;
        }
    }
}

