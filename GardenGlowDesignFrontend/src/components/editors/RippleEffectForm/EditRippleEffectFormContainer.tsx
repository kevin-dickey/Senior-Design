// CreateRainbowEffectFormContainer.tsx
import React from 'react';
import {Effect, RippleEffect} from '../../serialization/Effect';
import {RippleEffectFormPresentational} from './RippleEffectFormPresentational';

interface EditRippleEffectFormContainerProps {
    effect: RippleEffect;
    onSubmit: (effect: Effect) => void;
    onDelete?: (effectId: number) => void;
    onClose?: () => void;
}

export const EditRippleEffectFormContainer:
    React.FC<EditRippleEffectFormContainerProps> = (props) => {
    return <RippleEffectFormPresentational {...props} />
};
