// CreateRainbowEffectFormContainer.tsx
import React from 'react';
import {Effect, RainbowEffect} from '../../serialization/Effect';
import { RainbowEffectFormPresentational } from './RainbowEffectFormPresentational';

interface RainbowEffectFormContainerProps {
  onSubmit: (effect: Effect) => void;
  onClose?: () => void;
}

export const CreateRainbowEffectFormContainer:
    React.FC<RainbowEffectFormContainerProps> = ({ onSubmit }) => {
  const handleSubmit = (values: Effect) => {
    // Handle form submission logic here
    onSubmit(values);
  };

  const effect = RainbowEffect.emptyEffect();
  return <RainbowEffectFormPresentational effect={effect} onSubmit={handleSubmit} />;
};
