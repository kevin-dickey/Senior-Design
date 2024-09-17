// CreateRainbowEffectFormContainer.tsx
import React from 'react';
import {Effect} from '../../serialization/Effect';
import {RainbowEffectFormPresentational} from './RainbowEffectFormPresentational';

interface EditRainbowEffectFormContainerProps {
    effect: Effect;
    onSubmit: (effect: Effect) => void;
    onDelete?: (effectId: number) => void;
    onClose?: () => void;
}

export const EditRainbowEffectFormContainer:
    React.FC<EditRainbowEffectFormContainerProps> = (props) => {
    return <RainbowEffectFormPresentational {...props} />
};
