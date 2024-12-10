import React from 'react';
import { Effect } from '../../serialization/Effect';
import { EffectsFormPresentational } from './EffectsFormPresentational';
import {EffectType} from '../../../types/index';
interface EffectFormContainerProps {
    effectType: EffectType;
    onSubmit: (effect: Effect) => void;
}

function formatEffectTypeName(effectType: EffectType): string {
    return effectType
      .replace(/([A-Z])/g, ' $1') 
      .replace(/^./, (str) => str.toUpperCase());
  }  

export const CreateEffectsFormContainer: 
    React.FC<EffectFormContainerProps> = ({ effectType, onSubmit }) => {
    const emptyEffect = Effect.emptyEffect(); 
    emptyEffect.type = effectType;
    emptyEffect.name = formatEffectTypeName(effectType);
    const handleSubmit = (values: Effect) => {
        onSubmit(values);
    };

    return <EffectsFormPresentational effect={emptyEffect} onSubmit={handleSubmit} />;
};
