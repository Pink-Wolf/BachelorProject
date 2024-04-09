import { Fragment } from "react"

function BasicInput({ type, value, field, setter }) {
    return (
        <input type={type} onChange={(e) => setter({ ...value, [field]: e.target.value })} />
    )
}
function TextAreaInput({ value, field, setter }) {
    return (
        <textarea onChange={(e) => setter({ ...value, [field]: e.target.value })} />
    )
}
function SelectInput({ value, field, setter, options }) {
    return (
        <select onChange={(e) => setter({ ...value, [field]: e.target.value })}>
            {options?.map((option, optionI) => {
                return <option key={optionI} value={option.value ?? option}>{option.name ?? option}</option>
            })}
        </select>
    )
}

function ArrayInput({ value, field, setter, forEach, newValue = () => "" }) {
    return (
        <ul>
            {value[field]?.map((x, i) => {
                return (
                    <li key={i}>
                        <input type="button" value="-"
                            onClick={(e) => setter({ ...value, [field]: value[field].filter((fx, fi) => i !== fi) })} />
                        {forEach(x, i, (newX) => setter({ ...value, [field]: value[field].with(i, newX) }))}
                    </li>
                )
            })}
            <li>
                <input type="button" value="+"
                    onClick={(e) => setter({ ...value, [field]: (value[field] ?? []).concat(newValue()) })} />
            </li>
        </ul>
    )
}

export function InputContainer({ value, field, setter, inputs }) {
    return inputs(value[field], (x) => setter({ ...value, [field]: x }))
}

export default function Input(props) {
    var InputComponent = BasicInput
    var multipleInputs = false
    switch (props.type) {
        case "textarea": InputComponent = TextAreaInput
            break;
        case "select": InputComponent = SelectInput
            break;
        case "array": InputComponent = ArrayInput
            multipleInputs = true
            break;
        case "container": InputComponent = InputContainer
            multipleInputs = true
        default: break;
    }

    const label = props.label ?? `${props.field[0].toUpperCase() + props.field.slice(1).replace(/([A-Z]+)/g, ' $1')}:`

    if (multipleInputs) return <section>{label}
        <InputComponent {...props} />
    </section>

    return <label>{label}
        <InputComponent {...props} />
    </label>
}
