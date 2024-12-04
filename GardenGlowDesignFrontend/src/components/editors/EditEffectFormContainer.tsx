import React from 'react';
import {Effect, RainbowEffect, RippleEffect} from '../serialization/Effect';
import {EditRainbowEffectFormContainer} from './RainbowEffectForm/EditRainbowEffectFormContainer';
import {EditRippleEffectFormContainer} from './RippleEffectForm/EditRippleEffectFormContainer';
import {EditEffectsFormContainer} from './EffectsForm/EditEffectsFormContainer'
import { EffectType } from '../../types';

interface EditEffectFormContainerProps {
    effect: Effect;
    onSubmit: (effect: Effect) => void;
    onDelete: (effectId: number) => void;
}

export const EditEffectFormContainer: React.FC<EditEffectFormContainerProps> =
    ({effect, onSubmit, onDelete}) => {
        switch (effect.type as keyof typeof EffectType) {
            case EffectType.rainbow:
                return <EditRainbowEffectFormContainer
                    effect={effect as RainbowEffect}
                    onSubmit={onSubmit}
                    onDelete={onDelete}
                />;
            case EffectType.ripple:
                return <EditRippleEffectFormContainer
                    effect={effect as RippleEffect}
                    onSubmit={onSubmit}
                    onDelete={onDelete}
                />;
                case EffectType.pumpkinRainbow:
                case EffectType.pumpkinRipple:
                case EffectType.ghostRainbow:
                case EffectType.ghostRipple:
                case EffectType.pumpkinGhostRainbow:
                case EffectType.pumpkinGhostRipple:
                case EffectType.snowflake:
                case EffectType.snowman:
                case EffectType.christmasTree:
                case EffectType.candyCane:
                    return <EditEffectsFormContainer
                        effect={effect}
                        onSubmit={onSubmit}
                        onDelete={onDelete} 
                    />
                // console.warn('Base effect constructor used:', effect);
                return null;
            default:
                console.error('Unknown effect type:', effect.constructor);
                return null;
        }
    };