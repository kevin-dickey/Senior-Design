import React from 'react';
import { Effect } from '../../serialization/Effect';
import { EffectsFormPresentational } from './EffectsFormPresentational';
import { EffectType } from '../../../types';

interface EditEffectsFormContainerProps {
    effect: Effect;
    onSubmit: (effect: Effect) => void;
    onDelete?: (effectId: number) => void;
}

export const EditEffectsFormContainer: React.FC<EditEffectsFormContainerProps> = ({ effect, onSubmit, onDelete }) => {
    return (
        <EffectsFormPresentational
            effect={effect}
            onSubmit={onSubmit}
            onDelete={onDelete}
        />
    );
};
