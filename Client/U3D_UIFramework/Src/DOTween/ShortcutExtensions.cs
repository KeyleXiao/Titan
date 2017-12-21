﻿// Author: Daniele Giardini - http://www.demigiant.com
// Created: 2014/07/28 10:40
//
// License Copyright (c) Daniele Giardini.
// This work is subject to the terms at http://dotween.demigiant.com/license.php

#if COMPATIBLE
using DG.Tweening.Core.Surrogates;
using DOVector3 = DG.Tweening.Core.Surrogates.Vector3Wrapper;
using DOQuaternion = DG.Tweening.Core.Surrogates.QuaternionWrapper;
#else
using DOVector3 = UnityEngine.Vector3;
using DOQuaternion = UnityEngine.Quaternion;
#endif
using DG.Tweening.Core;
using DG.Tweening.Core.Enums;
using DG.Tweening.CustomPlugins;
using DG.Tweening.Plugins;
using DG.Tweening.Plugins.Core.PathCore;
using DG.Tweening.Plugins.Options;
using UnityEngine;

#pragma warning disable 1573
namespace DG.Tweening
{
    /// <summary>
    /// Methods that extend known Unity objects and allow to directly create and control tweens from their instances
    /// </summary>
    public static partial class ShortcutExtensions
    {
        // ===================================================================================
        // CREATION SHORTCUTS ----------------------------------------------------------------

        #region Audio Shortcuts

        /// <summary>Tweens an AudioSource's volume to the given value.
        /// Also stores the AudioSource as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach (0 to 1)</param><param name="duration">The duration of the tween</param>
        public static Tweener DOFade(this AudioSource target, float endValue, float duration)
        {
            if (endValue < 0) endValue = 0;
            else if (endValue > 1) endValue = 1;
            return DOTween.To(() => target.volume, x => target.volume = x, endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens an AudioSource's pitch to the given value.
        /// Also stores the AudioSource as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOPitch(this AudioSource target, float endValue, float duration)
        {
            return DOTween.To(() => target.pitch, x => target.pitch = x, endValue, duration).SetTarget(target);
        }

        #endregion

        #region Camera Shortcuts

        /// <summary>Tweens a Camera's <code>aspect</code> to the given value.
        /// Also stores the camera as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOAspect(this Camera target, float endValue, float duration)
        {
            return DOTween.To(() => target.aspect, x => target.aspect = x, endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens a Camera's backgroundColor to the given value.
        /// Also stores the camera as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOColor(this Camera target, Color endValue, float duration)
        {
            return DOTween.To(() => target.backgroundColor, x => target.backgroundColor = x, endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens a Camera's <code>farClipPlane</code> to the given value.
        /// Also stores the camera as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOFarClipPlane(this Camera target, float endValue, float duration)
        {
            return DOTween.To(() => target.farClipPlane, x => target.farClipPlane = x, endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens a Camera's <code>fieldOfView</code> to the given value.
        /// Also stores the camera as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOFieldOfView(this Camera target, float endValue, float duration)
        {
            return DOTween.To(() => target.fieldOfView, x => target.fieldOfView = x, endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens a Camera's <code>nearClipPlane</code> to the given value.
        /// Also stores the camera as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DONearClipPlane(this Camera target, float endValue, float duration)
        {
            return DOTween.To(() => target.nearClipPlane, x => target.nearClipPlane = x, endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens a Camera's <code>orthographicSize</code> to the given value.
        /// Also stores the camera as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOOrthoSize(this Camera target, float endValue, float duration)
        {
            return DOTween.To(() => target.orthographicSize, x => target.orthographicSize = x, endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens a Camera's <code>pixelRect</code> to the given value.
        /// Also stores the camera as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOPixelRect(this Camera target, Rect endValue, float duration)
        {
            return DOTween.To(() => target.pixelRect, x => target.pixelRect = x, endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens a Camera's <code>rect</code> to the given value.
        /// Also stores the camera as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DORect(this Camera target, Rect endValue, float duration)
        {
            return DOTween.To(() => target.rect, x => target.rect = x, endValue, duration).SetTarget(target);
        }

        /// <summary>Shakes a Camera's localPosition along its relative X Y axes with the given values.
        /// Also stores the camera as the tween's target so it can be used for filtered operations</summary>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="strength">The shake strength</param>
        /// <param name="vibrato">Indicates how much will the shake vibrate</param>
        /// <param name="randomness">Indicates how much the shake will be random (0 to 180 - values higher than 90 kind of suck, so beware). 
        /// Setting it to 0 will shake along a single direction.</param>
        /// <param name="fadeOut">If TRUE the shake will automatically fadeOut smoothly within the tween's duration, otherwise it will not</param>
        public static Tweener DOShakePosition(this Camera target, float duration, float strength = 3, int vibrato = 10, float randomness = 90, bool fadeOut = true)
        {
            return DOTween.Shake(() => target.transform.localPosition, x => target.transform.localPosition = x, duration, strength, vibrato, randomness, true, fadeOut)
                .SetTarget(target).SetSpecialStartupMode(SpecialStartupMode.SetCameraShakePosition);
        }
        /// <summary>Shakes a Camera's localPosition along its relative X Y axes with the given values.
        /// Also stores the camera as the tween's target so it can be used for filtered operations</summary>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="strength">The shake strength on each axis</param>
        /// <param name="vibrato">Indicates how much will the shake vibrate</param>
        /// <param name="randomness">Indicates how much the shake will be random (0 to 180 - values higher than 90 kind of suck, so beware). 
        /// Setting it to 0 will shake along a single direction.</param>
        /// <param name="fadeOut">If TRUE the shake will automatically fadeOut smoothly within the tween's duration, otherwise it will not</param>
        public static Tweener DOShakePosition(this Camera target, float duration, Vector3 strength, int vibrato = 10, float randomness = 90, bool fadeOut = true)
        {
            return DOTween.Shake(() => target.transform.localPosition, x => target.transform.localPosition = x, duration, strength, vibrato, randomness, fadeOut)
                .SetTarget(target).SetSpecialStartupMode(SpecialStartupMode.SetCameraShakePosition);
        }

        /// <summary>Shakes a Camera's localRotation.
        /// Also stores the camera as the tween's target so it can be used for filtered operations</summary>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="strength">The shake strength</param>
        /// <param name="vibrato">Indicates how much will the shake vibrate</param>
        /// <param name="randomness">Indicates how much the shake will be random (0 to 180 - values higher than 90 kind of suck, so beware). 
        /// Setting it to 0 will shake along a single direction.</param>
        /// <param name="fadeOut">If TRUE the shake will automatically fadeOut smoothly within the tween's duration, otherwise it will not</param>
        public static Tweener DOShakeRotation(this Camera target, float duration, float strength = 90, int vibrato = 10, float randomness = 90, bool fadeOut = true)
        {
            return DOTween.Shake(() => target.transform.localEulerAngles, x => target.transform.localRotation = Quaternion.Euler(x), duration, strength, vibrato, randomness, false, fadeOut)
                .SetTarget(target).SetSpecialStartupMode(SpecialStartupMode.SetShake);
        }
        /// <summary>Shakes a Camera's localRotation.
        /// Also stores the camera as the tween's target so it can be used for filtered operations</summary>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="strength">The shake strength on each axis</param>
        /// <param name="vibrato">Indicates how much will the shake vibrate</param>
        /// <param name="randomness">Indicates how much the shake will be random (0 to 180 - values higher than 90 kind of suck, so beware). 
        /// Setting it to 0 will shake along a single direction.</param>
        /// <param name="fadeOut">If TRUE the shake will automatically fadeOut smoothly within the tween's duration, otherwise it will not</param>
        public static Tweener DOShakeRotation(this Camera target, float duration, Vector3 strength, int vibrato = 10, float randomness = 90, bool fadeOut = true)
        {
            return DOTween.Shake(() => target.transform.localEulerAngles, x => target.transform.localRotation = Quaternion.Euler(x), duration, strength, vibrato, randomness, fadeOut)
                .SetTarget(target).SetSpecialStartupMode(SpecialStartupMode.SetShake);
        }

        #endregion

        #region Light Shortcuts

        /// <summary>Tweens a Light's color to the given value.
        /// Also stores the light as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOColor(this Light target, Color endValue, float duration)
        {
            return DOTween.To(() => target.color, x => target.color = x, endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens a Light's intensity to the given value.
        /// Also stores the light as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOIntensity(this Light target, float endValue, float duration)
        {
            return DOTween.To(() => target.intensity, x => target.intensity = x, endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens a Light's shadowStrength to the given value.
        /// Also stores the light as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOShadowStrength(this Light target, float endValue, float duration)
        {
            return DOTween.To(() => target.shadowStrength, x => target.shadowStrength = x, endValue, duration).SetTarget(target);
        }

        #endregion

        #region LineRenderer

        /// <summary>Tweens a LineRenderer's color to the given value.
        /// Also stores the LineRenderer as the tween's target so it can be used for filtered operations.
        /// <para>Note that this method requires to also insert the start colors for the tween, 
        /// since LineRenderers have no way to get them.</para></summary>
        /// <param name="startValue">The start value to tween from</param>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOColor(this LineRenderer target, Color2 startValue, Color2 endValue, float duration)
        {
            return DOTween.To(() => startValue, x => target.SetColors(x.ca, x.cb), endValue, duration).SetTarget(target);
        }

        #endregion

        #region Material Shortcuts

        /// <summary>Tweens a Material's color to the given value.
        /// Also stores the material as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOColor(this Material target, Color endValue, float duration)
        {
            return DOTween.To(() => target.color, x => target.color = x, endValue, duration).SetTarget(target);
        }
        /// <summary>Tweens a Material's named color property to the given value.
        /// Also stores the material as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param>
        /// <param name="property">The name of the material property to tween (like _Tint or _SpecColor)</param>
        /// <param name="duration">The duration of the tween</param>
        public static Tweener DOColor(this Material target, Color endValue, string property, float duration)
        {
            if (!target.HasProperty(property)) {
                if (Debugger.logPriority > 0) Debugger.LogMissingMaterialProperty(property);
                return null;
            }
            return DOTween.To(() => target.GetColor(property), x => target.SetColor(property, x), endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens a Material's alpha color to the given value
        /// (will have no effect unless your material supports transparency).
        /// Also stores the material as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOFade(this Material target, float endValue, float duration)
        {
            return DOTween.ToAlpha(() => target.color, x => target.color = x, endValue, duration)
                .SetTarget(target);
        }
        /// <summary>Tweens a Material's alpha color to the given value
        /// (will have no effect unless your material supports transparency).
        /// Also stores the material as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param>
        /// <param name="property">The name of the material property to tween (like _Tint or _SpecColor)</param>
        /// <param name="duration">The duration of the tween</param>
        public static Tweener DOFade(this Material target, float endValue, string property, float duration)
        {
            if (!target.HasProperty(property)) {
                if (Debugger.logPriority > 0) Debugger.LogMissingMaterialProperty(property);
                return null;
            }
            return DOTween.ToAlpha(() => target.GetColor(property), x => target.SetColor(property, x), endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens a Material's named float property to the given value.
        /// Also stores the material as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param>
        /// <param name="property">The name of the material property to tween</param>
        /// <param name="duration">The duration of the tween</param>
        public static Tweener DOFloat(this Material target, float endValue, string property, float duration)
        {
            if (!target.HasProperty(property)) {
                if (Debugger.logPriority > 0) Debugger.LogMissingMaterialProperty(property);
                return null;
            }
            return DOTween.To(() => target.GetFloat(property), x => target.SetFloat(property, x), endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens a Material's texture offset to the given value.
        /// Also stores the material as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param>
        /// <param name="duration">The duration of the tween</param>
        public static Tweener DOOffset(this Material target, Vector2 endValue, float duration)
        {
            return DOTween.To(() => target.mainTextureOffset, x => target.mainTextureOffset = x, endValue, duration).SetTarget(target);
        }
        /// <summary>Tweens a Material's named texture offset property to the given value.
        /// Also stores the material as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param>
        /// <param name="property">The name of the material property to tween</param>
        /// <param name="duration">The duration of the tween</param>
        public static Tweener DOOffset(this Material target, Vector2 endValue, string property, float duration)
        {
            if (!target.HasProperty(property)) {
                if (Debugger.logPriority > 0) Debugger.LogMissingMaterialProperty(property);
                return null;
            }
            return DOTween.To(() => target.GetTextureOffset(property), x => target.SetTextureOffset(property, x), endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens a Material's texture scale to the given value.
        /// Also stores the material as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param>
        /// <param name="duration">The duration of the tween</param>
        public static Tweener DOTiling(this Material target, Vector2 endValue, float duration)
        {
            return DOTween.To(() => target.mainTextureScale, x => target.mainTextureScale = x, endValue, duration).SetTarget(target);
        }
        /// <summary>Tweens a Material's named texture scale property to the given value.
        /// Also stores the material as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param>
        /// <param name="property">The name of the material property to tween</param>
        /// <param name="duration">The duration of the tween</param>
        public static Tweener DOTiling(this Material target, Vector2 endValue, string property, float duration)
        {
            if (!target.HasProperty(property)) {
                if (Debugger.logPriority > 0) Debugger.LogMissingMaterialProperty(property);
                return null;
            }
            return DOTween.To(() => target.GetTextureScale(property), x => target.SetTextureScale(property, x), endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens a Material's named Vector property to the given value.
        /// Also stores the material as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param>
        /// <param name="property">The name of the material property to tween</param>
        /// <param name="duration">The duration of the tween</param>
        public static Tweener DOVector(this Material target, Vector4 endValue, string property, float duration)
        {
            if (!target.HasProperty(property)) {
                if (Debugger.logPriority > 0) Debugger.LogMissingMaterialProperty(property);
                return null;
            }
            return DOTween.To(() => target.GetVector(property), x => target.SetVector(property, x), endValue, duration).SetTarget(target);
        }

        #endregion

        #region Rigidbody Shortcuts

        /// <summary>Tweens a Rigidbody's position to the given value.
        /// Also stores the rigidbody as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOMove(this Rigidbody target, Vector3 endValue, float duration, bool snapping = false)
        {
#if COMPATIBLE
            return DOTween.To(() => target.position, x=> target.MovePosition(x.value), endValue, duration)
#else
            return DOTween.To(() => target.position, target.MovePosition, endValue, duration)
#endif
                .SetOptions(snapping).SetTarget(target);
        }

        /// <summary>Tweens a Rigidbody's X position to the given value.
        /// Also stores the rigidbody as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOMoveX(this Rigidbody target, float endValue, float duration, bool snapping = false)
        {
#if COMPATIBLE
            return DOTween.To(() => target.position, x => target.MovePosition(x.value), new Vector3(endValue, 0, 0), duration)
#else
            return DOTween.To(() => target.position, target.MovePosition, new Vector3(endValue, 0, 0), duration)
#endif
                .SetOptions(AxisConstraint.X, snapping).SetTarget(target);
        }

        /// <summary>Tweens a Rigidbody's Y position to the given value.
        /// Also stores the rigidbody as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOMoveY(this Rigidbody target, float endValue, float duration, bool snapping = false)
        {
#if COMPATIBLE
            return DOTween.To(() => target.position, x => target.MovePosition(x.value), new Vector3(0, endValue, 0), duration)
#else
            return DOTween.To(() => target.position, target.MovePosition, new Vector3(0, endValue, 0), duration)
#endif
                .SetOptions(AxisConstraint.Y, snapping).SetTarget(target);
        }

        /// <summary>Tweens a Rigidbody's Z position to the given value.
        /// Also stores the rigidbody as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOMoveZ(this Rigidbody target, float endValue, float duration, bool snapping = false)
        {
#if COMPATIBLE
            return DOTween.To(() => target.position, x => target.MovePosition(x.value), new Vector3(0, 0, endValue), duration)
#else
            return DOTween.To(() => target.position, target.MovePosition, new Vector3(0, 0, endValue), duration)
#endif
                .SetOptions(AxisConstraint.Z, snapping).SetTarget(target);
        }

        /// <summary>Tweens a Rigidbody's rotation to the given value.
        /// Also stores the rigidbody as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="mode">Rotation mode</param>
        public static Tweener DORotate(this Rigidbody target, Vector3 endValue, float duration, RotateMode mode = RotateMode.Fast)
        {
#if COMPATIBLE
            TweenerCore<QuaternionWrapper, Vector3Wrapper, QuaternionOptions> t = DOTween.To(() => target.rotation, x => target.MoveRotation(x), endValue, duration);
#else
            TweenerCore<Quaternion, Vector3, QuaternionOptions> t = DOTween.To(() => target.rotation, target.MoveRotation, endValue, duration);
#endif
            t.SetTarget(target);
            t.plugOptions.rotateMode = mode;
            return t;
        }

        /// <summary>Tweens a Rigidbody's rotation so that it will look towards the given position.
        /// Also stores the rigidbody as the tween's target so it can be used for filtered operations</summary>
        /// <param name="towards">The position to look at</param><param name="duration">The duration of the tween</param>
        /// <param name="axisConstraint">Eventual axis constraint for the rotation</param>
        /// <param name="up">The vector that defines in which direction up is (default: Vector3.up)</param>
        public static Tweener DOLookAt(this Rigidbody target, Vector3 towards, float duration, AxisConstraint axisConstraint = AxisConstraint.None, Vector3? up = null)
        {
#if COMPATIBLE
            TweenerCore<QuaternionWrapper, Vector3Wrapper, QuaternionOptions> t = DOTween.To(() => target.rotation, x => target.MoveRotation(x), towards, duration)
#else
            TweenerCore<Quaternion, Vector3, QuaternionOptions> t = DOTween.To(() => target.rotation, target.MoveRotation, towards, duration)
#endif
                .SetTarget(target).SetSpecialStartupMode(SpecialStartupMode.SetLookAt);
            t.plugOptions.axisConstraint = axisConstraint;
            t.plugOptions.up = (up == null) ? Vector3.up : (Vector3)up;
            return t;
        }

        #region Special

        /// <summary>Tweens a Rigidbody's position to the given value, while also applying a jump effect along the Y axis.
        /// Returns a Sequence instead of a Tweener.
        /// Also stores the Rigidbody as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param>
        /// <param name="jumpPower">Power of the jump (the max height of the jump is represented by this plus the final Y offset)</param>
        /// <param name="numJumps">Total number of jumps</param>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Sequence DOJump(this Rigidbody target, Vector3 endValue, float jumpPower, int numJumps, float duration, bool snapping = false)
        {
            if (numJumps < 1) numJumps = 1;
            float startPosY = target.position.y;
            float offsetY = -1;
            bool offsetYSet = false;
            Sequence s = DOTween.Sequence();
#if COMPATIBLE
            s.Append(DOTween.To(() => target.position, x => target.MovePosition(x.value), new Vector3(endValue.x, 0, 0), duration)
#else
            s.Append(DOTween.To(() => target.position, target.MovePosition, new Vector3(endValue.x, 0, 0), duration)
#endif
                    .SetOptions(AxisConstraint.X, snapping).SetEase(Ease.Linear)
                    .OnUpdate(() => {
                        if (!offsetYSet) {
                            offsetYSet = true;
                            offsetY = s.isRelative ? endValue.y : endValue.y - startPosY;
                        }
                        Vector3 pos = target.position;
                        pos.y += DOVirtual.EasedValue(0, offsetY, s.ElapsedDirectionalPercentage(), Ease.OutQuad);
                        target.MovePosition(pos);
                    })
#if COMPATIBLE
                ).Join(DOTween.To(() => target.position, x => target.MovePosition(x.value), new Vector3(0, 0, endValue.z), duration)
#else
                ).Join(DOTween.To(() => target.position, target.MovePosition, new Vector3(0, 0, endValue.z), duration)
#endif
                    .SetOptions(AxisConstraint.Z, snapping).SetEase(Ease.Linear)
#if COMPATIBLE
                ).Join(DOTween.To(() => target.position, x => target.MovePosition(x.value), new Vector3(0, jumpPower, 0), duration / (numJumps * 2))
#else
                ).Join(DOTween.To(() => target.position, target.MovePosition, new Vector3(0, jumpPower, 0), duration / (numJumps * 2))
#endif
                    .SetOptions(AxisConstraint.Y, snapping).SetEase(Ease.OutQuad)
                    .SetLoops(numJumps * 2, LoopType.Yoyo).SetRelative()
                ).SetTarget(target).SetEase(DOTween.defaultEaseType);
            return s;
        }

        #endregion

        #endregion

        #region TrailRenderer Shortcuts

        /// <summary>Tweens a TrailRenderer's startWidth/endWidth to the given value.
        /// Also stores the TrailRenderer as the tween's target so it can be used for filtered operations</summary>
        /// <param name="toStartWidth">The end startWidth to reach</param><param name="toEndWidth">The end endWidth to reach</param>
        /// <param name="duration">The duration of the tween</param>
        public static Tweener DOResize(this TrailRenderer target, float toStartWidth, float toEndWidth, float duration)
        {
            return DOTween.To(() => new Vector2(target.startWidth, target.endWidth), x => {
#if COMPATIBLE
                    target.startWidth = x.value.x;
                    target.endWidth = x.value.y;
#else
                    target.startWidth = x.x;
                    target.endWidth = x.y;
#endif
                }, new Vector2(toStartWidth, toEndWidth), duration)
                .SetTarget(target);
        }

        /// <summary>Tweens a TrailRenderer's time to the given value.
        /// Also stores the TrailRenderer as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOTime(this TrailRenderer target, float endValue, float duration)
        {
            return DOTween.To(() => target.time, x => target.time = x, endValue, duration)
                .SetTarget(target);
        }

        #endregion

        #region Transform Shortcuts

        /// <summary>Tweens a Transform's position to the given value.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOMove(this Transform target, Vector3 endValue, float duration, bool snapping = false)
        {
            return DOTween.To(() => target.position, x => target.position = x, endValue, duration)
                .SetOptions(snapping).SetTarget(target);
        }

        /// <summary>Tweens a Transform's X position to the given value.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOMoveX(this Transform target, float endValue, float duration, bool snapping = false)
        {
            return DOTween.To(() => target.position, x => target.position = x, new Vector3(endValue, 0, 0), duration)
                .SetOptions(AxisConstraint.X, snapping).SetTarget(target);
        }

        /// <summary>Tweens a Transform's Y position to the given value.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOMoveY(this Transform target, float endValue, float duration, bool snapping = false)
        {
            return DOTween.To(() => target.position, x => target.position = x, new Vector3(0, endValue, 0), duration)
                .SetOptions(AxisConstraint.Y, snapping).SetTarget(target);
        }

        /// <summary>Tweens a Transform's Z position to the given value.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOMoveZ(this Transform target, float endValue, float duration, bool snapping = false)
        {
            return DOTween.To(() => target.position, x => target.position = x, new Vector3(0, 0, endValue), duration)
                .SetOptions(AxisConstraint.Z, snapping).SetTarget(target);
        }

        /// <summary>Tweens a Transform's localPosition to the given value.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOLocalMove(this Transform target, Vector3 endValue, float duration, bool snapping = false)
        {
            return DOTween.To(() => target.localPosition, x => target.localPosition = x, endValue, duration)
                .SetOptions(snapping).SetTarget(target);
        }

        /// <summary>Tweens a Transform's X localPosition to the given value.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOLocalMoveX(this Transform target, float endValue, float duration, bool snapping = false)
        {
            return DOTween.To(() => target.localPosition, x => target.localPosition = x, new Vector3(endValue, 0, 0), duration)
                .SetOptions(AxisConstraint.X, snapping).SetTarget(target);
        }

        /// <summary>Tweens a Transform's Y localPosition to the given value.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOLocalMoveY(this Transform target, float endValue, float duration, bool snapping = false)
        {
            return DOTween.To(() => target.localPosition, x => target.localPosition = x, new Vector3(0, endValue, 0), duration)
                .SetOptions(AxisConstraint.Y, snapping).SetTarget(target);
        }

        /// <summary>Tweens a Transform's Z localPosition to the given value.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOLocalMoveZ(this Transform target, float endValue, float duration, bool snapping = false)
        {
            return DOTween.To(() => target.localPosition, x => target.localPosition = x, new Vector3(0, 0, endValue), duration)
                .SetOptions(AxisConstraint.Z, snapping).SetTarget(target);
        }

        /// <summary>Tweens a Transform's rotation to the given value.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="mode">Rotation mode</param>
        public static Tweener DORotate(this Transform target, Vector3 endValue, float duration, RotateMode mode = RotateMode.Fast)
        {
            TweenerCore<DOQuaternion, DOVector3, QuaternionOptions> t = DOTween.To(() => target.rotation, x => target.rotation = x, endValue, duration);
            t.SetTarget(target);
            t.plugOptions.rotateMode = mode;
            return t;
        }

        /// <summary>Tweens a Transform's rotation to the given value using pure quaternion values.
        /// Also stores the transform as the tween's target so it can be used for filtered operations.
        /// <para>PLEASE NOTE: DORotate, which takes Vector3 values, is the preferred rotation method.
        /// This method was implemented for very special cases, and doesn't support LoopType.Incremental loops
        /// (neither for itself nor if placed inside a LoopType.Incremental Sequence)</para>
        /// </summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DORotateQuaternion(this Transform target, Quaternion endValue, float duration)
        {
            TweenerCore<Quaternion, Quaternion, NoOptions> t = DOTween.To(PureQuaternionPlugin.Plug(), () => target.rotation, x => target.rotation = x, endValue, duration);
            t.SetTarget(target);
            return t;
        }

        /// <summary>Tweens a Transform's localRotation to the given value.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        /// <param name="mode">Rotation mode</param>
        public static Tweener DOLocalRotate(this Transform target, Vector3 endValue, float duration, RotateMode mode = RotateMode.Fast)
        {
            TweenerCore<DOQuaternion, DOVector3, QuaternionOptions> t = DOTween.To(() => target.localRotation, x => target.localRotation = x, endValue, duration);
            t.SetTarget(target);
            t.plugOptions.rotateMode = mode;
            return t;
        }

        /// <summary>Tweens a Transform's rotation to the given value using pure quaternion values.
        /// Also stores the transform as the tween's target so it can be used for filtered operations.
        /// <para>PLEASE NOTE: DOLocalRotate, which takes Vector3 values, is the preferred rotation method.
        /// This method was implemented for very special cases, and doesn't support LoopType.Incremental loops
        /// (neither for itself nor if placed inside a LoopType.Incremental Sequence)</para>
        /// </summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOLocalRotateQuaternion(this Transform target, Quaternion endValue, float duration)
        {
            TweenerCore<Quaternion, Quaternion, NoOptions> t = DOTween.To(PureQuaternionPlugin.Plug(), () => target.localRotation, x => target.localRotation = x, endValue, duration);
            t.SetTarget(target);
            return t;
        }

        /// <summary>Tweens a Transform's localScale to the given value.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOScale(this Transform target, Vector3 endValue, float duration)
        {
            return DOTween.To(() => target.localScale, x => target.localScale = x, endValue, duration).SetTarget(target);
        }

        /// <summary>Tweens a Transform's localScale uniformly to the given value.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOScale(this Transform target, float endValue, float duration)
        {
            Vector3 endValueV3 = new Vector3(endValue, endValue, endValue);
            return DOTween.To(() => target.localScale, x => target.localScale = x, endValueV3, duration).SetTarget(target);
        }

        /// <summary>Tweens a Transform's X localScale to the given value.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOScaleX(this Transform target, float endValue, float duration)
        {
            return DOTween.To(() => target.localScale, x => target.localScale = x, new Vector3(endValue, 0, 0), duration)
                .SetOptions(AxisConstraint.X)
                .SetTarget(target);
        }

        /// <summary>Tweens a Transform's Y localScale to the given value.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOScaleY(this Transform target, float endValue, float duration)
        {
            return DOTween.To(() => target.localScale, x => target.localScale = x, new Vector3(0, endValue, 0), duration)
                .SetOptions(AxisConstraint.Y)
                .SetTarget(target);
        }

        /// <summary>Tweens a Transform's Z localScale to the given value.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param><param name="duration">The duration of the tween</param>
        public static Tweener DOScaleZ(this Transform target, float endValue, float duration)
        {
            return DOTween.To(() => target.localScale, x => target.localScale = x, new Vector3(0, 0, endValue), duration)
                .SetOptions(AxisConstraint.Z)
                .SetTarget(target);
        }

        /// <summary>Tweens a Transform's rotation so that it will look towards the given position.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="towards">The position to look at</param><param name="duration">The duration of the tween</param>
        /// <param name="axisConstraint">Eventual axis constraint for the rotation</param>
        /// <param name="up">The vector that defines in which direction up is (default: Vector3.up)</param>
        public static Tweener DOLookAt(this Transform target, Vector3 towards, float duration, AxisConstraint axisConstraint = AxisConstraint.None, Vector3? up = null)
        {
            TweenerCore<DOQuaternion, DOVector3, QuaternionOptions> t = DOTween.To(() => target.rotation, x => target.rotation = x, towards, duration)
                .SetTarget(target).SetSpecialStartupMode(SpecialStartupMode.SetLookAt);
            t.plugOptions.axisConstraint = axisConstraint;
            t.plugOptions.up = (up == null) ? Vector3.up : (Vector3)up;
            return t;
        }

        /// <summary>Punches a Transform's localPosition towards the given direction and then back to the starting one
        /// as if it was connected to the starting position via an elastic.</summary>
        /// <param name="punch">The direction and strength of the punch (added to the Transform's current position)</param>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="vibrato">Indicates how much will the punch vibrate</param>
        /// <param name="elasticity">Represents how much (0 to 1) the vector will go beyond the starting position when bouncing backwards.
        /// 1 creates a full oscillation between the punch direction and the opposite direction,
        /// while 0 oscillates only between the punch and the start position</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOPunchPosition(this Transform target, Vector3 punch, float duration, int vibrato = 10, float elasticity = 1, bool snapping = false)
        {
            return DOTween.Punch(() => target.localPosition, x => target.localPosition = x, punch, duration, vibrato, elasticity)
                .SetTarget(target).SetOptions(snapping);
        }
        /// <summary>Punches a Transform's localScale towards the given size and then back to the starting one
        /// as if it was connected to the starting scale via an elastic.</summary>
        /// <param name="punch">The punch strength (added to the Transform's current scale)</param>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="vibrato">Indicates how much will the punch vibrate</param>
        /// <param name="elasticity">Represents how much (0 to 1) the vector will go beyond the starting size when bouncing backwards.
        /// 1 creates a full oscillation between the punch scale and the opposite scale,
        /// while 0 oscillates only between the punch scale and the start scale</param>
        public static Tweener DOPunchScale(this Transform target, Vector3 punch, float duration, int vibrato = 10, float elasticity = 1)
        {
            return DOTween.Punch(() => target.localScale, x => target.localScale = x, punch, duration, vibrato, elasticity)
                .SetTarget(target);
        }
        /// <summary>Punches a Transform's localRotation towards the given size and then back to the starting one
        /// as if it was connected to the starting rotation via an elastic.</summary>
        /// <param name="punch">The punch strength (added to the Transform's current rotation)</param>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="vibrato">Indicates how much will the punch vibrate</param>
        /// <param name="elasticity">Represents how much (0 to 1) the vector will go beyond the starting rotation when bouncing backwards.
        /// 1 creates a full oscillation between the punch rotation and the opposite rotation,
        /// while 0 oscillates only between the punch and the start rotation</param>
        public static Tweener DOPunchRotation(this Transform target, Vector3 punch, float duration, int vibrato = 10, float elasticity = 1)
        {
            return DOTween.Punch(() => target.localEulerAngles, x => target.localRotation = Quaternion.Euler(x), punch, duration, vibrato, elasticity)
                .SetTarget(target);
        }

        /// <summary>Shakes a Transform's localPosition with the given values.</summary>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="strength">The shake strength</param>
        /// <param name="vibrato">Indicates how much will the shake vibrate</param>
        /// <param name="randomness">Indicates how much the shake will be random (0 to 180 - values higher than 90 kind of suck, so beware). 
        /// Setting it to 0 will shake along a single direction.</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        /// <param name="fadeOut">If TRUE the shake will automatically fadeOut smoothly within the tween's duration, otherwise it will not</param>
        public static Tweener DOShakePosition(this Transform target, float duration, float strength = 1, int vibrato = 10, float randomness = 90, bool snapping = false, bool fadeOut = true)
        {
            return DOTween.Shake(() => target.localPosition, x => target.localPosition = x, duration, strength, vibrato, randomness, false, fadeOut)
                .SetTarget(target).SetSpecialStartupMode(SpecialStartupMode.SetShake).SetOptions(snapping);
        }
        /// <summary>Shakes a Transform's localPosition with the given values.</summary>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="strength">The shake strength on each axis</param>
        /// <param name="vibrato">Indicates how much will the shake vibrate</param>
        /// <param name="randomness">Indicates how much the shake will be random (0 to 180 - values higher than 90 kind of suck, so beware). 
        /// Setting it to 0 will shake along a single direction.</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        /// <param name="fadeOut">If TRUE the shake will automatically fadeOut smoothly within the tween's duration, otherwise it will not</param>
        public static Tweener DOShakePosition(this Transform target, float duration, Vector3 strength, int vibrato = 10, float randomness = 90, bool snapping = false, bool fadeOut = true)
        {
            return DOTween.Shake(() => target.localPosition, x => target.localPosition = x, duration, strength, vibrato, randomness, fadeOut)
                .SetTarget(target).SetSpecialStartupMode(SpecialStartupMode.SetShake).SetOptions(snapping);
        }
        /// <summary>Shakes a Transform's localRotation.</summary>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="strength">The shake strength</param>
        /// <param name="vibrato">Indicates how much will the shake vibrate</param>
        /// <param name="randomness">Indicates how much the shake will be random (0 to 180 - values higher than 90 kind of suck, so beware). 
        /// Setting it to 0 will shake along a single direction.</param>
        /// <param name="fadeOut">If TRUE the shake will automatically fadeOut smoothly within the tween's duration, otherwise it will not</param>
        public static Tweener DOShakeRotation(this Transform target, float duration, float strength = 90, int vibrato = 10, float randomness = 90, bool fadeOut = true)
        {
            return DOTween.Shake(() => target.localEulerAngles, x => target.localRotation = Quaternion.Euler(x), duration, strength, vibrato, randomness, false, fadeOut)
                .SetTarget(target).SetSpecialStartupMode(SpecialStartupMode.SetShake);
        }
        /// <summary>Shakes a Transform's localRotation.</summary>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="strength">The shake strength on each axis</param>
        /// <param name="vibrato">Indicates how much will the shake vibrate</param>
        /// <param name="randomness">Indicates how much the shake will be random (0 to 180 - values higher than 90 kind of suck, so beware). 
        /// Setting it to 0 will shake along a single direction.</param>
        /// <param name="fadeOut">If TRUE the shake will automatically fadeOut smoothly within the tween's duration, otherwise it will not</param>
        public static Tweener DOShakeRotation(this Transform target, float duration, Vector3 strength, int vibrato = 10, float randomness = 90, bool fadeOut = true)
        {
            return DOTween.Shake(() => target.localEulerAngles, x => target.localRotation = Quaternion.Euler(x), duration, strength, vibrato, randomness, fadeOut)
                .SetTarget(target).SetSpecialStartupMode(SpecialStartupMode.SetShake);
        }
        /// <summary>Shakes a Transform's localScale.</summary>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="strength">The shake strength</param>
        /// <param name="vibrato">Indicates how much will the shake vibrate</param>
        /// <param name="randomness">Indicates how much the shake will be random (0 to 180 - values higher than 90 kind of suck, so beware). 
        /// Setting it to 0 will shake along a single direction.</param>
        /// <param name="fadeOut">If TRUE the shake will automatically fadeOut smoothly within the tween's duration, otherwise it will not</param>
        public static Tweener DOShakeScale(this Transform target, float duration, float strength = 1, int vibrato = 10, float randomness = 90, bool fadeOut = true)
        {
            return DOTween.Shake(() => target.localScale, x => target.localScale = x, duration, strength, vibrato, randomness, false, fadeOut)
                .SetTarget(target).SetSpecialStartupMode(SpecialStartupMode.SetShake);
        }
        /// <summary>Shakes a Transform's localScale.</summary>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="strength">The shake strength on each axis</param>
        /// <param name="vibrato">Indicates how much will the shake vibrate</param>
        /// <param name="randomness">Indicates how much the shake will be random (0 to 180 - values higher than 90 kind of suck, so beware). 
        /// Setting it to 0 will shake along a single direction.</param>
        /// <param name="fadeOut">If TRUE the shake will automatically fadeOut smoothly within the tween's duration, otherwise it will not</param>
        public static Tweener DOShakeScale(this Transform target, float duration, Vector3 strength, int vibrato = 10, float randomness = 90, bool fadeOut = true)
        {
            return DOTween.Shake(() => target.localScale, x => target.localScale = x, duration, strength, vibrato, randomness, fadeOut)
                .SetTarget(target).SetSpecialStartupMode(SpecialStartupMode.SetShake);
        }

        #region Special

        /// <summary>Tweens a Transform's position to the given value, while also applying a jump effect along the Y axis.
        /// Returns a Sequence instead of a Tweener.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param>
        /// <param name="jumpPower">Power of the jump (the max height of the jump is represented by this plus the final Y offset)</param>
        /// <param name="numJumps">Total number of jumps</param>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Sequence DOJump(this Transform target, Vector3 endValue, float jumpPower, int numJumps, float duration, bool snapping = false)
        {
            if (numJumps < 1) numJumps = 1;
            float startPosY = target.position.y;
            float offsetY = -1;
            bool offsetYSet = false;


            Sequence s = DOTween.Sequence();
            s.Append(DOTween.To(() => target.position, x => target.position = x, new Vector3(endValue.x, 0, 0), duration)
                    .SetOptions(AxisConstraint.X, snapping).SetEase(Ease.Linear)
                    .OnUpdate(() => {
                        if (!offsetYSet) {
                            offsetYSet = true;
                            offsetY = s.isRelative ? endValue.y : endValue.y - startPosY;
                        }
                        Vector3 pos = target.position;
                        pos.y += DOVirtual.EasedValue(0, offsetY, s.ElapsedDirectionalPercentage(), Ease.OutQuad);
                        target.position = pos;
                    })
                ).Join(DOTween.To(() => target.position, x => target.position = x, new Vector3(0, 0, endValue.z), duration)
                    .SetOptions(AxisConstraint.Z, snapping).SetEase(Ease.Linear)
                ).Join(DOTween.To(() => target.position, x => target.position = x, new Vector3(0, jumpPower, 0), duration / (numJumps * 2))
                    .SetOptions(AxisConstraint.Y, snapping).SetEase(Ease.OutQuad).SetRelative()
                    .SetLoops(numJumps * 2, LoopType.Yoyo)
                ).SetTarget(target).SetEase(DOTween.defaultEaseType);
            return s;
        }
        /// <summary>Tweens a Transform's localPosition to the given value, while also applying a jump effect along the Y axis.
        /// Returns a Sequence instead of a Tweener.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The end value to reach</param>
        /// <param name="jumpPower">Power of the jump (the max height of the jump is represented by this plus the final Y offset)</param>
        /// <param name="numJumps">Total number of jumps</param>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Sequence DOLocalJump(this Transform target, Vector3 endValue, float jumpPower, int numJumps, float duration, bool snapping = false)
        {
            if (numJumps < 1) numJumps = 1;
            float startPosY = target.localPosition.y;
            float offsetY = -1;
            bool offsetYSet = false;
            Sequence s = DOTween.Sequence();
            s.Append(DOTween.To(() => target.localPosition, x => target.localPosition = x, new Vector3(endValue.x, 0, 0), duration)
                    .SetOptions(AxisConstraint.X, snapping).SetEase(Ease.Linear)
                    .OnUpdate(() => {
                        if (!offsetYSet) {
                            offsetYSet = false;
                            offsetY = s.isRelative ? endValue.y : endValue.y - startPosY;
                        }
                        Vector3 pos = target.localPosition;
                        pos.y += DOVirtual.EasedValue(0, offsetY, s.ElapsedDirectionalPercentage(), Ease.OutQuad);
                        target.localPosition = pos;
                    })
                ).Join(DOTween.To(() => target.localPosition, x => target.localPosition = x, new Vector3(0, 0, endValue.z), duration)
                    .SetOptions(AxisConstraint.Z, snapping).SetEase(Ease.Linear)
                ).Join(DOTween.To(() => target.localPosition, x => target.localPosition = x, new Vector3(0, jumpPower, 0), duration / (numJumps * 2))
                    .SetOptions(AxisConstraint.Y, snapping).SetEase(Ease.OutQuad).SetRelative()
                    .SetLoops(numJumps * 2, LoopType.Yoyo)
                ).SetTarget(target).SetEase(DOTween.defaultEaseType);
            return s;
        }

        /// <summary>Tweens a Transform's position through the given path waypoints, using the chosen path algorithm.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="path">The waypoints to go through</param>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="pathType">The type of path: Linear (straight path) or CatmullRom (curved CatmullRom path)</param>
        /// <param name="pathMode">The path mode: 3D, side-scroller 2D, top-down 2D</param>
        /// <param name="resolution">The resolution of the path (useless in case of Linear paths): higher resolutions make for more detailed curved paths but are more expensive.
        /// Defaults to 10, but a value of 5 is usually enough if you don't have dramatic long curves between waypoints</param>
        /// <param name="gizmoColor">The color of the path (shown when gizmos are active in the Play panel and the tween is running)</param>
        public static TweenerCore<Vector3, Path, PathOptions> DOPath(
            this Transform target, Vector3[] path, float duration, PathType pathType = PathType.Linear,
            PathMode pathMode = PathMode.Full3D, int resolution = 10, Color? gizmoColor = null
        )
        {
            if (resolution < 1) resolution = 1;
            TweenerCore<Vector3, Path, PathOptions> t = DOTween.To(PathPlugin.Get(), () => target.position, x => target.position = x, new Path(pathType, path, resolution, gizmoColor), duration)
                .SetTarget(target);

            t.plugOptions.mode = pathMode;
            return t;
        }
        /// <summary>Tweens a Transform's localPosition through the given path waypoints, using the chosen path algorithm.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="path">The waypoint to go through</param>
        /// <param name="duration">The duration of the tween</param>
        /// <param name="pathType">The type of path: Linear (straight path) or CatmullRom (curved CatmullRom path)</param>
        /// <param name="pathMode">The path mode: 3D, side-scroller 2D, top-down 2D</param>
        /// <param name="resolution">The resolution of the path: higher resolutions make for more detailed curved paths but are more expensive.
        /// Defaults to 10, but a value of 5 is usually enough if you don't have dramatic long curves between waypoints</param>
        /// <param name="gizmoColor">The color of the path (shown when gizmos are active in the Play panel and the tween is running)</param>
        public static TweenerCore<Vector3, Path, PathOptions> DOLocalPath(
            this Transform target, Vector3[] path, float duration, PathType pathType = PathType.Linear,
            PathMode pathMode = PathMode.Full3D, int resolution = 10, Color? gizmoColor = null
        )
        {
            if (resolution < 1) resolution = 1;
            TweenerCore<Vector3, Path, PathOptions> t = DOTween.To(PathPlugin.Get(), () => target.localPosition, x => target.localPosition = x, new Path(pathType, path, resolution, gizmoColor), duration)
                .SetTarget(target);

            t.plugOptions.mode = pathMode;
            t.plugOptions.useLocalPosition = true;
            return t;
        }
        // Used by path editor when creating the actual tween, so it can pass a pre-compiled path
        internal static TweenerCore<Vector3, Path, PathOptions> DOPath(
            this Transform target, Path path, float duration, PathMode pathMode = PathMode.Full3D
        )
        {
            TweenerCore<Vector3, Path, PathOptions> t = DOTween.To(PathPlugin.Get(), () => target.position, x => target.position = x, path, duration)
                .SetTarget(target);

            t.plugOptions.mode = pathMode;
            return t;
        }
        internal static TweenerCore<Vector3, Path, PathOptions> DOLocalPath(
            this Transform target, Path path, float duration, PathMode pathMode = PathMode.Full3D
        )
        {
            TweenerCore<Vector3, Path, PathOptions> t = DOTween.To(PathPlugin.Get(), () => target.localPosition, x => target.localPosition = x, path, duration)
                .SetTarget(target);

            t.plugOptions.mode = pathMode;
            t.plugOptions.useLocalPosition = true;
            return t;
        }

        #endregion

        #endregion

        #region Blendables

        #region Light

        /// <summary>Tweens a Light's color to the given value,
        /// in a way that allows other DOBlendableColor tweens to work together on the same target,
        /// instead than fight each other as multiple DOColor would do.
        /// Also stores the Light as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The value to tween to</param><param name="duration">The duration of the tween</param>
        public static Tweener DOBlendableColor(this Light target, Color endValue, float duration)
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

        #region Material

        /// <summary>Tweens a Material's color to the given value,
        /// in a way that allows other DOBlendableColor tweens to work together on the same target,
        /// instead than fight each other as multiple DOColor would do.
        /// Also stores the Material as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The value to tween to</param><param name="duration">The duration of the tween</param>
        public static Tweener DOBlendableColor(this Material target, Color endValue, float duration)
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
        /// <summary>Tweens a Material's named color property to the given value,
        /// in a way that allows other DOBlendableColor tweens to work together on the same target,
        /// instead than fight each other as multiple DOColor would do.
        /// Also stores the Material as the tween's target so it can be used for filtered operations</summary>
        /// <param name="endValue">The value to tween to</param>
        /// <param name="property">The name of the material property to tween (like _Tint or _SpecColor)</param>
        /// <param name="duration">The duration of the tween</param>
        public static Tweener DOBlendableColor(this Material target, Color endValue, string property, float duration)
        {
            if (!target.HasProperty(property)) {
                if (Debugger.logPriority > 0) Debugger.LogMissingMaterialProperty(property);
                return null;
            }

            endValue = endValue - target.color;
            Color to = new Color(0, 0, 0, 0);
            return DOTween.To(() => to, x => {
#if COMPATIBLE
                Color diff = x.value - to;
#else
                Color diff = x - to;
#endif
                to = x;
                target.SetColor(property, target.GetColor(property) + diff);
            }, endValue, duration)
                .Blendable().SetTarget(target);
        }

        #endregion

        #region Transform

        /// <summary>Tweens a Transform's position BY the given value (as if you chained a <code>SetRelative</code>),
        /// in a way that allows other DOBlendableMove tweens to work together on the same target,
        /// instead than fight each other as multiple DOMove would do.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="byValue">The value to tween by</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOBlendableMoveBy(this Transform target, Vector3 byValue, float duration, bool snapping = false)
        {
            Vector3 to = Vector3.zero;
            return DOTween.To(() => to, x => {
#if COMPATIBLE
                Vector3 diff = x.value - to;
#else
                Vector3 diff = x - to;
#endif
                to = x;
                target.position += diff;
            }, byValue, duration)
                .Blendable().SetOptions(snapping).SetTarget(target);
        }

        /// <summary>Tweens a Transform's localPosition BY the given value (as if you chained a <code>SetRelative</code>),
        /// in a way that allows other DOBlendableMove tweens to work together on the same target,
        /// instead than fight each other as multiple DOMove would do.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="byValue">The value to tween by</param><param name="duration">The duration of the tween</param>
        /// <param name="snapping">If TRUE the tween will smoothly snap all values to integers</param>
        public static Tweener DOBlendableLocalMoveBy(this Transform target, Vector3 byValue, float duration, bool snapping = false)
        {
            Vector3 to = Vector3.zero;
            return DOTween.To(() => to, x => {
#if COMPATIBLE
                Vector3 diff = x.value - to;
#else
                Vector3 diff = x - to;
#endif
                to = x;
                target.localPosition += diff;
            }, byValue, duration)
                .Blendable().SetOptions(snapping).SetTarget(target);
        }

        /// <summary>EXPERIMENTAL METHOD - Tweens a Transform's rotation BY the given value (as if you chained a <code>SetRelative</code>),
        /// in a way that allows other DOBlendableRotate tweens to work together on the same target,
        /// instead than fight each other as multiple DORotate would do.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="byValue">The value to tween by</param><param name="duration">The duration of the tween</param>
        /// <param name="mode">Rotation mode</param>
        public static Tweener DOBlendableRotateBy(this Transform target, Vector3 byValue, float duration, RotateMode mode = RotateMode.Fast)
        {
            Quaternion to = target.rotation;
            TweenerCore<DOQuaternion, DOVector3, QuaternionOptions> t = DOTween.To(() => to, x => {
#if COMPATIBLE
                Quaternion diff = x.value * Quaternion.Inverse(to);
#else
                Quaternion diff = x * Quaternion.Inverse(to);
#endif
                to = x;
                target.rotation = target.rotation * Quaternion.Inverse(target.rotation) * diff * target.rotation;
            }, byValue, duration)
                .Blendable().SetTarget(target);
            t.plugOptions.rotateMode = mode;
            return t;
        }

        /// <summary>EXPERIMENTAL METHOD - Tweens a Transform's lcoalRotation BY the given value (as if you chained a <code>SetRelative</code>),
        /// in a way that allows other DOBlendableRotate tweens to work together on the same target,
        /// instead than fight each other as multiple DORotate would do.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="byValue">The value to tween by</param><param name="duration">The duration of the tween</param>
        /// <param name="mode">Rotation mode</param>
        public static Tweener DOBlendableLocalRotateBy(this Transform target, Vector3 byValue, float duration, RotateMode mode = RotateMode.Fast)
        {
            Quaternion to = target.localRotation;
            TweenerCore<DOQuaternion, DOVector3, QuaternionOptions> t = DOTween.To(() => to, x => {
#if COMPATIBLE
                Quaternion diff = x.value * Quaternion.Inverse(to);
#else
                Quaternion diff = x * Quaternion.Inverse(to);
#endif
                to = x;
                target.localRotation = target.localRotation * Quaternion.Inverse(target.localRotation) * diff * target.localRotation;
            }, byValue, duration)
                .Blendable().SetTarget(target);
            t.plugOptions.rotateMode = mode;
            return t;
        }

        /// <summary>Tweens a Transform's localScale BY the given value (as if you chained a <code>SetRelative</code>),
        /// in a way that allows other DOBlendableScale tweens to work together on the same target,
        /// instead than fight each other as multiple DOScale would do.
        /// Also stores the transform as the tween's target so it can be used for filtered operations</summary>
        /// <param name="byValue">The value to tween by</param><param name="duration">The duration of the tween</param>
        public static Tweener DOBlendableScaleBy(this Transform target, Vector3 byValue, float duration)
        {
            Vector3 to = Vector3.zero;
            return DOTween.To(() => to, x => {
#if COMPATIBLE
                Vector3 diff = x.value - to;
#else
                Vector3 diff = x - to;
#endif
                to = x;
                target.localScale += diff;
            }, byValue, duration)
                .Blendable().SetTarget(target);
        }

        #endregion

        #endregion

        // ===================================================================================
        // OPERATION SHORTCUTS ---------------------------------------------------------------

        #region Operation Shortcuts

        /// <summary>
        /// Completes all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens completed
        /// (meaning the tweens that don't have infinite loops and were not already complete)
        /// </summary>
        /// <param name="withCallbacks">For Sequences only: if TRUE also internal Sequence callbacks will be fired,
        /// otherwise they will be ignored</param>
        public static int DOComplete(this Component target, bool withCallbacks = false)
        {
            return DOTween.Complete(target, withCallbacks);
        }
        /// <summary>
        /// Completes all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens completed
        /// (meaning the tweens that don't have infinite loops and were not already complete)
        /// </summary>
        /// <param name="withCallbacks">For Sequences only: if TRUE also internal Sequence callbacks will be fired,
        /// otherwise they will be ignored</param>
        public static int DOComplete(this Material target, bool withCallbacks = false)
        {
            return DOTween.Complete(target, withCallbacks);
        }

        /// <summary>
        /// Kills all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens killed.
        /// </summary>
        /// <param name="complete">If TRUE completes the tween before killing it</param>
        public static int DOKill(this Component target, bool complete = false)
        {
//            int tot = complete ? DOTween.CompleteAndReturnKilledTot(target) : 0;
//            return tot + DOTween.Kill(target);
            return DOTween.Kill(target, complete);
        }
        /// <summary>
        /// Kills all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens killed.
        /// </summary>
        /// <param name="complete">If TRUE completes the tween before killing it</param>
        public static int DOKill(this Material target, bool complete = false)
        {
            return DOTween.Kill(target, complete);
        }

        /// <summary>
        /// Flips the direction (backwards if it was going forward or viceversa) of all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens flipped.
        /// </summary>
        public static int DOFlip(this Component target)
        {
            return DOTween.Flip(target);
        }
        /// <summary>
        /// Flips the direction (backwards if it was going forward or viceversa) of all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens flipped.
        /// </summary>
        public static int DOFlip(this Material target)
        {
            return DOTween.Flip(target);
        }

        /// <summary>
        /// Sends to the given position all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens involved.
        /// </summary>
        /// <param name="to">Time position to reach
        /// (if higher than the whole tween duration the tween will simply reach its end)</param>
        /// <param name="andPlay">If TRUE will play the tween after reaching the given position, otherwise it will pause it</param>
        public static int DOGoto(this Component target, float to, bool andPlay = false)
        {
            return DOTween.Goto(target, to, andPlay);
        }
        /// <summary>
        /// Sends to the given position all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens involved.
        /// </summary>
        /// <param name="to">Time position to reach
        /// (if higher than the whole tween duration the tween will simply reach its end)</param>
        /// <param name="andPlay">If TRUE will play the tween after reaching the given position, otherwise it will pause it</param>
        public static int DOGoto(this Material target, float to, bool andPlay = false)
        {
            return DOTween.Goto(target, to, andPlay);
        }

        /// <summary>
        /// Pauses all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens paused.
        /// </summary>
        public static int DOPause(this Component target)
        {
            return DOTween.Pause(target);
        }
        /// <summary>
        /// Pauses all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens paused.
        /// </summary>
        public static int DOPause(this Material target)
        {
            return DOTween.Pause(target);
        }

        /// <summary>
        /// Plays all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens played.
        /// </summary>
        public static int DOPlay(this Component target)
        {
            return DOTween.Play(target);
        }
        /// <summary>
        /// Plays all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens played.
        /// </summary>
        public static int DOPlay(this Material target)
        {
            return DOTween.Play(target);
        }

        /// <summary>
        /// Plays backwards all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens played.
        /// </summary>
        public static int DOPlayBackwards(this Component target)
        {
            return DOTween.PlayBackwards(target);
        }
        /// <summary>
        /// Plays backwards all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens played.
        /// </summary>
        public static int DOPlayBackwards(this Material target)
        {
            return DOTween.PlayBackwards(target);
        }

        /// <summary>
        /// Plays forward all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens played.
        /// </summary>
        public static int DOPlayForward(this Component target)
        {
            return DOTween.PlayForward(target);
        }
        /// <summary>
        /// Plays forward all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens played.
        /// </summary>
        public static int DOPlayForward(this Material target)
        {
            return DOTween.PlayForward(target);
        }

        /// <summary>
        /// Restarts all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens restarted.
        /// </summary>
        public static int DORestart(this Component target, bool includeDelay = true)
        {
            return DOTween.Restart(target, includeDelay);
        }
        /// <summary>
        /// Restarts all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens restarted.
        /// </summary>
        public static int DORestart(this Material target, bool includeDelay = true)
        {
            return DOTween.Restart(target, includeDelay);
        }

        /// <summary>
        /// Rewinds all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens rewinded.
        /// </summary>
        public static int DORewind(this Component target, bool includeDelay = true)
        {
            return DOTween.Rewind(target, includeDelay);
        }
        /// <summary>
        /// Rewinds all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens rewinded.
        /// </summary>
        public static int DORewind(this Material target, bool includeDelay = true)
        {
            return DOTween.Rewind(target, includeDelay);
        }

        /// <summary>
        /// Smoothly rewinds all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens rewinded.
        /// </summary>
        public static int DOSmoothRewind(this Component target)
        {
            return DOTween.SmoothRewind(target);
        }
        /// <summary>
        /// Smoothly rewinds all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens rewinded.
        /// </summary>
        public static int DOSmoothRewind(this Material target)
        {
            return DOTween.SmoothRewind(target);
        }

        /// <summary>
        /// Toggles the paused state (plays if it was paused, pauses if it was playing) of all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens involved.
        /// </summary>
        public static int DOTogglePause(this Component target)
        {
            return DOTween.TogglePause(target);
        }
        /// <summary>
        /// Toggles the paused state (plays if it was paused, pauses if it was playing) of all tweens that have this target as a reference
        /// (meaning tweens that were started from this target, or that had this target added as an Id)
        /// and returns the total number of tweens involved.
        /// </summary>
        public static int DOTogglePause(this Material target)
        {
            return DOTween.TogglePause(target);
        }

        #endregion
    }
}