// CreateRainbowEffectFormContainer.tsx
import React from 'react';
import {Effect} from '../../serialization/Effect';
import { RainbowEffectFormPresentational } from './RainbowEffectFormPresentational';

interface EditRainbowEffectFormContainerProps {
  effect: Effect;
  onSubmit: (effect: Effect) => void;
}

export const EditRainbowEffectFormContainer:
    React.FC<EditRainbowEffectFormContainerProps> = ({ effect, onSubmit }) => {
  const handleSubmit = (values: Effect) => {
    // Handle form submission logic here
    onSubmit(values);
  };

  return <RainbowEffectFormPresentational effect={effect} onSubmit={handleSubmit} />;
};
