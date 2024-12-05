import React from 'react';
import { ErrorMessage, Field } from 'formik';
import { EffectForm } from '../EffectForms';
import { Effect } from '../../serialization/Effect';
import Grid from "@mui/material/Grid";

interface RippleEffectFormProps {
    effect: Effect;
    onSubmit: (effect: Effect) => void;
    onDelete?: (effectId: number) => void;
}

export const RippleEffectFormPresentational:
    React.FC<RippleEffectFormProps> = (props) => {
        const labelColumns = 7;
        const fieldColumns = 12 - labelColumns;
        return (
            <EffectForm {...props}>
                <Grid item xs={labelColumns}>
                    <label htmlFor="rippleSpeed">Speed</label>
                </Grid>
                {/* TODO: Add fields for color */}
                <Grid item xs={fieldColumns}>
                    <Field id="speed"
                        name="speed"
                        type="number"
                        placeholder="Enter value (1-5)"
                        min="1"
                        max="5"
                        step="1"
                        validate={(value: string | number | undefined) => {
                            if (!value) return "Speed is required";
                            if (typeof value === "number" && value < 1) return "Speed must be at least 1";
                            if (!Number.isInteger(Number(value))) return "Speed must be an integer";
                            if (typeof value === "number" && value > 5) return "Speed must be less than 6";
                            return undefined;
                        }}
                    />
                    <ErrorMessage name="speed" component="div" />
                </Grid>
            </EffectForm>
        );
    };
