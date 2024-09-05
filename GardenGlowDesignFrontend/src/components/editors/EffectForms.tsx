import React from 'react';
import { Formik, Form, Field } from 'formik';
import { Effect } from '../serialization/Effect';

interface EffectFormProps {
  effect: Effect;
  onSubmit: (effect: Effect) => void;
  children?: React.ReactNode;
}

export const EffectForm: React.FC<EffectFormProps> = ({ effect, onSubmit, children }) => {
  return (
    <Formik
      initialValues={effect}
      onSubmit={(values) => {
        onSubmit(values);
      }}
    >
      {() => (
        <Form>
          <div>
            <label htmlFor="name">Name</label>
            <Field id="name" name="name" placeholder="Effect Name" />
          </div>
          <div>
            <label htmlFor="start">Start</label>
            <Field id="startTimeMs" name="startTimeMs" type="number" />
          </div>
          <div>
            <label htmlFor="duration">End</label>
            <Field id="duration" name="duration" type="number" />
          </div>
          <div>
            <label htmlFor="layer">Layer</label>
            <Field id="layer" name="layer" type="number" />
          </div>
          {children}
          <button type="submit">Save</button>
        </Form>
      )}
    </Formik>
  );
};
