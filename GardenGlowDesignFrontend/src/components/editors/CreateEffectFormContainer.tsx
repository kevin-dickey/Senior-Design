import React from 'react';
import {Effect} from '../serialization/Effect';
import {
    CreateRainbowEffectFormContainer
} from './RainbowEffectForm/CreateRainbowEffectFormContainer';
import {CreateRippleEffectFormContainer} from "./RippleEffectForm/CreateRippleEffectFormContainer";

interface CreateEffectFormContainerProps {
    effectType: string;
    onSubmit: (effect: Effect) => void;
}

// TODO: This can probably be collapsed into the EditEffectForm container which will handle both
//  creation and editing.
export const CreateEffectFormContainer: React.FC<CreateEffectFormContainerProps> =
    ({effectType, onSubmit}) => {
        switch (effectType) {
            case 'RainbowEffect':
                return (
                    <CreateRainbowEffectFormContainer onSubmit={onSubmit} />
                );
            case 'RippleEffect':
                return (
                    <CreateRippleEffectFormContainer onSubmit={onSubmit} />
                );
            default:
                return null;
        }
    };