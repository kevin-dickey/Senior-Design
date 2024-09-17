import React from 'react';
import {Effect, RippleEffect} from '../../serialization/Effect';
import {RippleEffectFormPresentational} from './RippleEffectFormPresentational';

interface RippleEffectFormContainerProps {
    onSubmit: (effect: Effect) => void;
    onClose?: () => void;
}

export const CreateRippleEffectFormContainer:
    React.FC<RippleEffectFormContainerProps> = ({onSubmit, onClose}) => {
    const handleSubmit = (values: Effect) => {
        // Handle form submission logic here
        onSubmit(values);
    };

    const effect = RippleEffect.emptyEffect();
    return <RippleEffectFormPresentational
        effect={effect}
        onSubmit={handleSubmit}
        onClose={onClose}
    />;
};
