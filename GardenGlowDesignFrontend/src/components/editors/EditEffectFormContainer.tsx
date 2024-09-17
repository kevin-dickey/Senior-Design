import React from 'react';
import {Effect, RainbowEffect, RippleEffect} from '../serialization/Effect';
import {EditRainbowEffectFormContainer} from './RainbowEffectForm/EditRainbowEffectFormContainer';
import {EditRippleEffectFormContainer} from './RippleEffectForm/EditRippleEffectFormContainer';

interface EditEffectFormContainerProps {
    effect: Effect;
    onSubmit: (effect: Effect) => void;
    onDelete: (effectId: number) => void;
    onClose?: () => void;
}

export const EditEffectFormContainer: React.FC<EditEffectFormContainerProps> =
    ({effect, onSubmit, onDelete, onClose}) => {
        switch (effect.constructor) {
            case RainbowEffect:
                return <EditRainbowEffectFormContainer
                    effect={effect as RainbowEffect}
                    onSubmit={onSubmit}
                    onDelete={onDelete}
                    onClose={onClose}
                />;
            case RippleEffect:
                return <EditRippleEffectFormContainer
                    effect={effect as RippleEffect}
                    onSubmit={onSubmit}
                    onDelete={onDelete}
                    onClose={onClose}
                />;
            default:
                return null;
        }
    };