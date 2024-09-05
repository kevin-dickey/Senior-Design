// RainbowEffectForm.tsx
import React from 'react';
import { Field } from 'formik';
import { EffectForm } from '../EffectForms';
import { Effect } from '../../serialization/Effect';

interface RainbowEffectFormProps {
  effect: Effect;
  onSubmit: (effect: Effect) => void;
}

export const RainbowEffectFormPresentational:
    React.FC<RainbowEffectFormProps> = ({ effect, onSubmit }) => {
  return (
    <EffectForm effect={effect} onSubmit={onSubmit}>
      <div>
        <label htmlFor="rainbowSpeed">Rainbow Speed</label>
        <Field id="speed" name="speed" type="number" />
      </div>
    </EffectForm>
  );
};
