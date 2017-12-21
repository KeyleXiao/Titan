﻿/*******************************************************************
** 文件名:	ShortcutExtensions_43.cs 
** 描  述:	该文件原本属于DoTween项目DOTween43.dll模块
********************************************************************/
// Author: Daniele Giardini - http://www.demigiant.com
// Created: 2014/09/04 17:55
// 
// License Copyright (c) Daniele Giardini.
// This work is subject to the terms at http://dotween.demigiant.com/license.php

using DG.Tweening.Core;
using DG.Tweening.Core.Enums;
using DG.Tweening.Plugins.Options;
using UnityEngine;

namespace DG.Tweening
{
    /// <summary>
    /// Methods that extend known Unity objects and allow to directly create and control tweens from their instances.
    /// These, as all DOTween43 methods, require Unity 4.3 or later.
    /// </summary>
    public static partial class ShortcutExtensions
    {
        #region Material

        /// <summary>Tweens a Material's color using the given gradient
        /// (NOTE 1: only uses the colors of the gradient, not the alphas - NOTE 2: creates a Sequence, not a Tweener).
        /// Also stores the image as the tween's target so it can be used for filtered operations</summary>
        /// <param name="gradient">The gradient to use</param><param name="duration">The duration of the tween</param>
        public static Sequence DOGradientColor(this Material target, Gradient gradient, float duration)
        {
            Sequence s = DOTween.Sequence();
            GradientColorKey[] colors = gradient.colorKeys;
            int len = colors.Length;
            for (int i = 0; i < len; ++i) {
                GradientColorKey c = colors[i];
                if (i == 0 && c.time <= 0) {
                    target.color = c.color;
                    continue;
                }
                float colorDuration = i == len - 1
                    ? duration - s.Duration(false) // Verifies that total duration is correct
                    : duration * (i == 0 ? c.time : c.time - colors[i - 1].time);
                s.Append(target.DOColor(c.color, colorDuration).SetEase(Ease.Linear));
            }
            return s;
        }
        /// <summary>Tweens a Material's named color property using the given gradient
        /// (NOTE 1: only uses the colors of the gradient, not the alphas - NOTE 2: creates a Sequence, not a Tweener).
        /// Also stores the image as the tween's target so it can be used for filtered operations</summary>
        /// <param name="gradient">The gradient to use</param>
        /// <param name="property">The name of the material property to tween (like _Tint or _SpecColor)</param>
        /// <param name="duration">The duration of the tween</param>
        public static Sequence DOGradientColor(this Material target, Gradient gradient, string property, float duration)
        {
            Sequence s = DOTween.Sequence();
            GradientColorKey[] colors = gradient.colorKeys;
            int len = colors.Length;
            for (int i = 0; i < len; ++i) {
                GradientColorKey c = colors[i];
                if (i == 0 && c.time <= 0) {
                    target.color = c.color;
                    continue;
                }
                float colorDuration = i == len - 1
                    ? duration - s.Duration(false) // Verifies that total duration is correct
                    : duration * (i == 0 ? c.time : c.time - colors[i - 1].time);
                s.Append(target.DOColor(c.color, property, colorDuration).SetEase(Ease.Linear));
            }
            return s;
        }

        #endregion

        #region SpriteRenderer

        /// <summary>Tweens a SpriteRenderer's color to the given value.
        /// Also stores the spriteRenderer as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOColor(this SpriteRenderer target, Color endValue, float duration)
        {
            return DOTween.To(() => target.color, x => target.color = x, endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens a Material's alpha color to the given value.
        /// Also stores the spriteRenderer as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOFade(this SpriteRenderer target, float endValue, float duration)
        {
            return DOTween.ToAlpha(() => target.color, x => target.color = x, endValue, duration)
                .SetTarget(target);
        }

        /// <summary>Tweens a SpriteRenderer's color using the given gradient
        /// (NOTE 1: only uses the colors of the gradient, not the alphas - NOTE 2: creates a Sequence, not a Tweener).
        /// Also stores the image as the tween's target so it can be used for filtered operations</summary>
        /// <param name="gradient">The gradient to use</param><param name="duration">The duration of the tween</param>
        public static Sequence DOGradientColor(this SpriteRenderer target, Gradient gradient, float duration)
        {
            Sequence s = DOTween.Sequence();
            GradientColorKey[] colors = gradient.colorKeys;
            int len = colors.Length;
            for (int i = 0; i < len; ++i) {
                GradientColorKey c = colors[i];
                if (i == 0 && c.time <= 0) {
                    target.color = c.color;
                    continue;
                }
                float colorDuration = i == len - 1
                    ? duration - s.Duration(false) // Verifies that total duration is correct
                    : duration * (i == 0 ? c.time : c.time - colors[i - 1].time);
                s.Append(target.DOColor(c.color, colorDuration).SetEase(Ease.Linear));
            }
            return s;
        }

        #endregion

        #region Rigidbody2D Shortcuts

        /// <summary>Tweens a Rigidbody2D's position to the given value.
        /// Also stores the Rigidbody2D as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOMove(this Rigidbody2D target, Vector2 endValue, float duration, bool snapping = false)
        {
#if COMPATIBLE
            return DOTween.To(() => target.position, x=> target.MovePosition(x.value), endValue, duration)
#else
            return DOTween.To(() => target.position, target.MovePosition, endValue, duration)
#endif
                .SetOptions(snapping).SetTarget(target);
        }

        /// <summary>Tweens a Rigidbody2D's X position to the given value.
        /// Also stores the Rigidbody2D as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOMoveX(this Rigidbody2D target, float endValue, float duration, bool snapping = false)
        {
#if COMPATIBLE
            return DOTween.To(() => target.position, x => target.MovePosition(x.value), new Vector2(endValue, 0), duration)
#else
            return DOTween.To(() => target.position, target.MovePosition, new Vector2(endValue, 0), duration)
#endif
                .SetOptions(AxisConstraint.X, snapping).SetTarget(target);
        }

        /// <summary>Tweens a Rigidbody2D's Y position to the given value.
        /// Also stores the Rigidbody2D as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOMoveY(this Rigidbody2D target, float endValue, float duration, bool snapping = false)
        {
#if COMPATIBLE
            return DOTween.To(() => target.position, x => target.MovePosition(x.value), new Vector2(0, endValue), duration)
#else
            return DOTween.To(() => target.position, target.MovePosition, new Vector2(0, endValue), duration)
#endif
                .SetOptions(AxisConstraint.Y, snapping).SetTarget(target);
        }

        /// <summary>Tweens a Rigidbody2D's rotation to the given value.
        /// Also stores the Rigidbody2D as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DORotate(this Rigidbody2D target, float endValue, float duration)
        {
            return DOTween.To(() => target.rotation, target.MoveRotation, endValue, duration)
                .SetTarget(target);
        }

        #region Special

        /// <summary>Tweens a Rigidbody2D's position to the given value, while also applying a jump effect along the Y axis.
        /// Returns a Sequence instead of a Tweener.
        /// Also stores the Rigidbody2D as the tween's target so it can be used for filtered operations.
        /// <para>IMPORTANT: a rigidbody2D can't be animated in a jump arc using MovePosition, so the tween will directly set the position</para></summary>
        /// <param name="endValue">The end value to reach</param>
        /// <param name="jumpPower">Power of the jump (the max height of the jump is represented by this plus the final Y offset)</param>
        /// <param name="numJumps">Total number of jumps</param>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Sequence DOJump(this Rigidbody2D target, Vector2 endValue, float jumpPower, int numJumps, float duration, bool snapping = false)
        {
            if (numJumps < 1) numJumps = 1;
            float startPosY = target.position.y;
            float offsetY = -1;
            bool offsetYSet = false;
            Sequence s = DOTween.Sequence();
#if COMPATIBLE
            s.Append(DOTween.To(() => target.position, x => target.position = x.value, new Vector3(endValue.x, 0, 0), duration)
#else
            s.Append(DOTween.To(() => target.position, x => target.position = x, new Vector2(endValue.x, 0), duration)
#endif
                    .SetOptions(AxisConstraint.X, snapping).SetEase(Ease.Linear)
                    .OnUpdate(() => {
                        if (!offsetYSet) {
                            offsetYSet = true;
                            offsetY = s.isRelative ? endValue.y : endValue.y - startPosY;
                        }
                        Vector2 pos = target.position;
                        pos.y += DOVirtual.EasedValue(0, offsetY, s.ElapsedDirectionalPercentage(), Ease.OutQuad);
                        target.position = pos;
                    })
#if COMPATIBLE
                ).Join(DOTween.To(() => target.position, x => target.position = x.value, new Vector3(0, jumpPower, 0), duration / (numJumps * 2))
#else
                ).Join(DOTween.To(() => target.position, x=> target.position = x, new Vector2(0, jumpPower), duration / (numJumps * 2))
#endif
                    .SetOptions(AxisConstraint.Y, snapping).SetEase(Ease.OutQuad)
                    .SetLoops(numJumps * 2, LoopType.Yoyo).SetRelative()
                ).SetTarget(target).SetEase(DOTween.defaultEaseType);
            return s;
        }

        #endregion

        #endregion

        #region Blendables

        #region SpriteRenderer

        /// <summary>Tweens a SpriteRenderer's color to the given value,
        /// in a way that allows other DOBlendableColor tweens to work together on the same target,
        /// instead than fight each other as multiple DOColor would do.
        /// Also stores the SpriteRenderer as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The value to tween to</param><param name="duration">The duration of the tween</param>
        public static Tweener DOBlendableColor(this SpriteRenderer target, Color endValue, float duration)
        {
            endValue = endValue - target.color;
            Color to = new Color(0, 0, 0, 0);
            return DOTween.To(() => to, x => {
#if COMPATIBLE
                Color diff = x.value - to;
#else
                Color diff = x - to;
#endif
                to = x;
                target.color += diff;
            }, endValue, duration)
                .Blendable().SetTarget(target);
        }

        #endregion

        #endregion
    }
}